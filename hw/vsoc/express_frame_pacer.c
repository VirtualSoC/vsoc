#include "hw/vsoc/express_frame_pacer.h"

#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/express_log.h"
#include "hw/vsoc/gpu/egl_trans.h"
#include "glad/glad.h"
#include "qemu/osdep.h"
#include "qemu/thread.h"
#include <glib.h>
#include <string.h>

#define PACER_HISTORY 256
#define USEC_PER_SEC 1000000.0

typedef struct FramePacerState {
    double prev_sleep_us;
    double prev_err_us;
    double k_us_per_unit;
    double ratios[PACER_HISTORY];
    gint64 times[PACER_HISTORY];
    int count;
    int head;
} FramePacerState;

static FramePacerConfig g_cfg = {
    .kp = 0.1,
    .ki = 0.01,
    .window_sec = 1.0,
    .custom_offset_us = 0.0,
};

static GHashTable *g_states = NULL;
static QemuMutex g_lock;
static gsize g_init_once = 0;
static GPrivate g_query_tls = G_PRIVATE_INIT(NULL);

typedef struct FramePacerQueryState {
    GLuint query_id;
    bool active;
} FramePacerQueryState;

typedef struct FramePacerGlobalK {
    double ratios[PACER_HISTORY];
    int count;
    int head;
    double k_us_per_unit;
} FramePacerGlobalK;

static FramePacerGlobalK g_parent_k = {0};
static double g_worker_k_cached = 0.0;
static frame_pacer_stats_sink g_stats_sink = NULL;

typedef struct PacerStats {
    gint64 last_log_us;
    double ratio_sum;
    double sleep_sum;
    double ratio_min;
    double ratio_max;
    uint64_t frames;
} PacerStats;

static PacerStats g_stats = {0};

static bool frame_pacer_is_egl_swap(uint64_t id)
{
    if (GET_DEVICE_ID(id) != EXPRESS_GPU_DEVICE_ID) {
        return false;
    }
    uint32_t fid = GET_FUN_ID(id);
    return fid == GET_FUN_ID(FUNID_eglSwapBuffers) || fid == GET_FUN_ID(FUNID_eglSwapBuffers_sync);
}

static FramePacerQueryState *frame_pacer_get_query_state(void)
{
    FramePacerQueryState *qs = (FramePacerQueryState *)g_private_get(&g_query_tls);
    if (!qs) {
        qs = g_new0(FramePacerQueryState, 1);
        g_private_set(&g_query_tls, qs);
    }
    return qs;
}

static double frame_pacer_estimate_complexity(void)
{
    FramePacerQueryState *qs = frame_pacer_get_query_state();
    GLuint64 invocations = 0;

    if (qs->query_id == 0) {
        glGenQueries(1, &qs->query_id);
    }

    if (qs->active) {
        glEndQuery(GL_FRAGMENT_SHADER_INVOCATIONS_ARB);
        glGetQueryObjectui64v(qs->query_id, GL_QUERY_RESULT, &invocations);
    }

    glBeginQuery(GL_FRAGMENT_SHADER_INVOCATIONS_ARB, qs->query_id);
    qs->active = true;

    if (invocations == 0) {
        return 1.0;
    }
    return (double)invocations;
}

static uint64_t frame_pacer_stream_key(const Thread_Context *ctx)
{
    uint64_t lo = ctx ? (uint64_t)(uint32_t)ctx->thread_id : 0;
    uint64_t hi = ctx ? (uint64_t)(uint32_t)ctx->process_id : 0;
    return (hi << 32) | lo;
}

static uint64_t frame_pacer_get_frame_time(const Thread_Context *ctx) {
    if (!ctx->device_id == EXPRESS_GPU_DEVICE_ID) {
        LOGW("frame_pacer_get_frame_time: called for non-GPU device %" PRIu64, ctx->device_id);
        return 0;
    }
    Render_Thread_Context *render_ctx = (Render_Thread_Context *)ctx;
    Window_Buffer *surface = render_ctx->render_double_buffer_draw;
    if (!surface) {
        LOGW("frame_pacer_get_frame_time: no surface for context %p", ctx);
        return 0;
    }
    if (surface->swap_time_cnt == 0) {
        return 0;
    }
    return surface->swap_time[(surface->swap_loc - 1) % 5];
}

void frame_pacer_maybe_apply(Thread_Context *context, uint64_t id)
{
    if (!g_ops.express_frame_pacer) {
        return;
    }
    if (!context || context->proxy) {
        return;
    }
    if (!frame_pacer_is_egl_swap(id)) {
        return;
    }
    double complexity = frame_pacer_estimate_complexity();
    uint64_t frame_time = frame_pacer_get_frame_time(context);
    frame_pacer_on_frame(frame_pacer_stream_key(context), complexity, frame_time);
}

static void frame_pacer_init_once(void)
{
    if (g_once_init_enter(&g_init_once)) {
        g_states = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);
        qemu_mutex_init(&g_lock);
        g_once_init_leave(&g_init_once, 1);
    }
}

static FramePacerState *frame_pacer_get_state(uint64_t key)
{
    frame_pacer_init_once();
    qemu_mutex_lock(&g_lock);
    FramePacerState *st = (FramePacerState *)g_hash_table_lookup(g_states, (gpointer)(uintptr_t)key);
    if (!st) {
        st = g_new0(FramePacerState, 1);
        st->k_us_per_unit = 0.0;
        g_hash_table_insert(g_states, (gpointer)(uintptr_t)key, st);
    }
    qemu_mutex_unlock(&g_lock);
    return st;
}

static double frame_pacer_min_ratio(FramePacerState *st, gint64 now_us, double window_us)
{
    double min_ratio = 0.0;
    for (int i = 0; i < st->count; ++i) {
        int idx = (st->head - 1 - i + PACER_HISTORY) % PACER_HISTORY;
        if (now_us - st->times[idx] > (gint64)window_us) {
            continue;
        }
        double r = st->ratios[idx];
        if (min_ratio == 0.0 || r < min_ratio) {
            min_ratio = r;
        }
    }
    return min_ratio;
}

void frame_pacer_configure(const FramePacerConfig *cfg)
{
    frame_pacer_init_once();
    qemu_mutex_lock(&g_lock);
    if (cfg) {
        g_cfg.kp = cfg->kp;
        g_cfg.ki = cfg->ki;
        g_cfg.window_sec = cfg->window_sec;
        g_cfg.custom_offset_us = cfg->custom_offset_us;
    } else {
        g_cfg.kp = 0.1;
        g_cfg.ki = 0.01;
        g_cfg.window_sec = 1.0;
        g_cfg.custom_offset_us = 0.0;
    }
    qemu_mutex_unlock(&g_lock);
}

static void frame_pacer_maybe_log_stats(gint64 now_us, double ratio, double sleep_us)
{
    const gint64 LOG_INTERVAL_US = 1000000; // ~1s
    if (g_stats.frames == 0) {
        g_stats.ratio_min = 0.0;
        g_stats.ratio_max = 0.0;
    }
    g_stats.frames += 1;
    g_stats.ratio_sum += ratio;
    g_stats.sleep_sum += sleep_us;
    if (g_stats.ratio_min == 0.0 || ratio < g_stats.ratio_min) g_stats.ratio_min = ratio;
    if (ratio > g_stats.ratio_max) g_stats.ratio_max = ratio;

    if (g_stats.last_log_us == 0) {
        g_stats.last_log_us = now_us;
        return;
    }
    if (now_us - g_stats.last_log_us < LOG_INTERVAL_US) {
        return;
    }

    double avg_ratio = g_stats.frames ? g_stats.ratio_sum / (double)g_stats.frames : 0.0;
    double avg_sleep = g_stats.frames ? g_stats.sleep_sum / (double)g_stats.frames : 0.0;
    double k = frame_pacer_worker_get_k();
    LOGI("frame_pacer: frames=%" PRIu64 " ratio[min/avg/max]=%.3f/%.3f/%.3f sleep_avg_us=%.3f k=%.3f", g_stats.frames, g_stats.ratio_min, avg_ratio, g_stats.ratio_max, avg_sleep, k);

    g_stats.last_log_us = now_us;
    g_stats.ratio_sum = 0.0;
    g_stats.sleep_sum = 0.0;
    g_stats.ratio_min = 0.0;
    g_stats.ratio_max = 0.0;
    g_stats.frames = 0;
}

double frame_pacer_on_frame(uint64_t stream_key, double complexity, uint64_t t_real_us)
{
    FramePacerState *st = frame_pacer_get_state(stream_key);
    if (complexity <= 0.0 || t_real_us == 0) {
        return 0.0;
    }

    gint64 now_us = g_get_monotonic_time();
    double sleep_us = 0.0;
    double ratio = 0.0;

    ratio = t_real_us / complexity;

    st->times[st->head] = now_us;
    st->ratios[st->head] = ratio;
    st->head = (st->head + 1) % PACER_HISTORY;
    if (st->count < PACER_HISTORY) {
        st->count++;
    }

    double k_us = g_worker_k_cached;
    if (k_us <= 0.0) {
        double window_us = (g_cfg.window_sec > 0.0 ? g_cfg.window_sec : 1.0) * USEC_PER_SEC;
        k_us = frame_pacer_min_ratio(st, now_us, window_us);
    }
    if (k_us == 0.0) {
        k_us = ratio;
    }
    st->k_us_per_unit = k_us;

    double t_est_us = complexity * k_us;
    double err = t_est_us - t_real_us + g_cfg.custom_offset_us;

    // Clamp extreme error swings to avoid wind-up that can produce multi-hundred-millisecond sleeps on light frames.
    const double MAX_ERR_US = 50000.0;  // 50 ms
    if (err > MAX_ERR_US) err = MAX_ERR_US;
    if (err < -MAX_ERR_US) err = -MAX_ERR_US;

    sleep_us = st->prev_sleep_us + g_cfg.kp * (err - st->prev_err_us) + g_cfg.ki * err;
    if (sleep_us < 0.0) {
        sleep_us = 0.0;
    }

    // Cap the output as a safety valve; pacer should never intentionally sleep more than a frame budget.
    const double MAX_SLEEP_US = 50000.0; // 50 ms
    if (sleep_us > MAX_SLEEP_US) {
        sleep_us = MAX_SLEEP_US;
    }

    st->prev_err_us = err;
    st->prev_sleep_us = sleep_us;
    qemu_mutex_unlock(&g_lock);

    if (g_stats_sink) {
        LOGI("worker: reporting ratio %.6f", ratio);
        g_stats_sink(ratio);
    }

    frame_pacer_maybe_log_stats(now_us, ratio, sleep_us);

    if (sleep_us > 0.0) {
        g_usleep((gulong)sleep_us);
    }
    return sleep_us;
}

void frame_pacer_set_stats_sink(frame_pacer_stats_sink sink)
{
    g_stats_sink = sink;
}

void frame_pacer_parent_ingest_ratio(double ratio)
{
    if (ratio <= 0.0) {
        return;
    }
    frame_pacer_init_once();
    qemu_mutex_lock(&g_lock);
    g_parent_k.ratios[g_parent_k.head] = ratio;
    g_parent_k.head = (g_parent_k.head + 1) % PACER_HISTORY;
    if (g_parent_k.count < PACER_HISTORY) {
        g_parent_k.count++;
    }

    // global K is the min of recent ratios (most conservative estimate)
    double k_us = 0.0;
    for (int i = 0; i < g_parent_k.count; ++i) {
        int idx = (g_parent_k.head - 1 - i + PACER_HISTORY) % PACER_HISTORY;
        double r = g_parent_k.ratios[idx];
        if (k_us == 0.0 || r < k_us) {
            k_us = r;
        }
    }
    g_parent_k.k_us_per_unit = k_us;
    qemu_mutex_unlock(&g_lock);
}

double frame_pacer_parent_current_k(void)
{
    frame_pacer_init_once();
    qemu_mutex_lock(&g_lock);
    double k = g_parent_k.k_us_per_unit;
    qemu_mutex_unlock(&g_lock);
    return k;
}

void frame_pacer_worker_set_k(double k)
{
    frame_pacer_init_once();
    qemu_mutex_lock(&g_lock);
    LOGI("worker set k: %.6f->%.6f", g_worker_k_cached, k);
    g_worker_k_cached = k;
    qemu_mutex_unlock(&g_lock);
}

double frame_pacer_worker_get_k(void)
{
    frame_pacer_init_once();
    qemu_mutex_lock(&g_lock);
    double k = g_worker_k_cached;
    qemu_mutex_unlock(&g_lock);
    return k;
}
