// #define STD_DEBUG_LOG
#include "hw/vsoc/express_frame_pacer.h"

#include "hw/vsoc/express_platform.h"
#include "hw/vsoc/express_log.h"
#include "hw/vsoc/gpu/egl_trans.h"
#include "glad/glad.h"
#include "qemu/osdep.h"
#include "qemu/thread.h"
#include <glib.h>
#include <math.h>
#include <string.h>

#define PACER_HISTORY 256
#define USEC_PER_SEC 1000000.0
#define PACER_QUERY_BUFFERS 3       /* triple-buffer pipeline-stat queries so results can be read without stalling the GPU */
#define PACER_DEFAULT_ALPHA 1.0     /* vertex-invocation -> fragment-equivalent weight (calibrate per-GPU later) */
#define PACER_DEFAULT_BETA 1.0      /* compute-invocation -> fragment-equivalent weight (calibrate per-GPU later) */
#define PACER_MIN_PERCENTILE 0.05   /* robust minimum: use 5th percentile so transient low outliers cannot collapse k */

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
    .alpha = PACER_DEFAULT_ALPHA,
    .beta = PACER_DEFAULT_BETA,
};

static GHashTable *g_states = NULL;
static QemuMutex g_lock;
static gsize g_init_once = 0;
static GPrivate g_query_tls = G_PRIVATE_INIT(NULL);

typedef enum FramePacerQueryKind {
    PACER_Q_FRAG = 0,
    PACER_Q_VERT,
    PACER_Q_COMP,
    PACER_Q_KIND_COUNT
} FramePacerQueryKind;

/*
 * One pipeline-statistics counter, triple-buffered. Each frame we end the
 * query begun last frame and read back the result that was ended two frames
 * ago (guaranteed ready), so the readback never blocks the render thread.
 */
typedef struct FramePacerQuery {
    GLuint ids[PACER_QUERY_BUFFERS];
    bool ended[PACER_QUERY_BUFFERS]; /* slot has an ended query whose result is unread */
    int active;                      /* slot currently between Begin/End, -1 if none */
    int write;                       /* next slot to begin a query on */
    GLuint64 last_value;             /* most recent successfully read counter value */
} FramePacerQuery;

typedef struct FramePacerQueryState {
    FramePacerQuery q[PACER_Q_KIND_COUNT];
    bool generated;
} FramePacerQueryState;

typedef struct FramePacerGlobalK {
    double ratios[PACER_HISTORY];
    gint64 times[PACER_HISTORY];
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
    return fid == GET_FUN_ID(FUNID_eglSwapBuffers);
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

/*
 * Advance one triple-buffered pipeline-statistics query and return the latest
 * available counter value without stalling. Steps, per call (frame):
 *   1. end the query begun on the previous frame;
 *   2. non-blocking read of the slot we are about to reuse (ended >=2 frames
 *      ago, so its result is ready); keep the previous value if somehow not;
 *   3. begin a fresh query on that slot.
 */
static GLuint64 frame_pacer_query_step(FramePacerQuery *fq, GLenum target)
{
    if (fq->active >= 0) {
        glEndQuery(target);
        fq->ended[fq->active] = true;
        fq->active = -1;
    }

    int slot = fq->write;
    if (fq->ended[slot]) {
        GLuint available = 0;
        glGetQueryObjectuiv(fq->ids[slot], GL_QUERY_RESULT_AVAILABLE, &available);
        if (available) {
            glGetQueryObjectui64v(fq->ids[slot], GL_QUERY_RESULT, &fq->last_value);
        }
        fq->ended[slot] = false;
    }

    glBeginQuery(target, fq->ids[slot]);
    fq->active = slot;
    fq->write = (fq->write + 1) % PACER_QUERY_BUFFERS;

    return fq->last_value;
}

/*
 * Composite frame complexity C = N_frag + alpha*N_vert + beta*N_comp, gathered
 * from GPU pipeline-statistics queries. Counters are read asynchronously, so
 * the values correspond to a frame rendered a couple of frames earlier; that
 * small lag is harmless given the sliding-window smoothing downstream.
 */
static double frame_pacer_estimate_complexity(void)
{
    FramePacerQueryState *qs = frame_pacer_get_query_state();

    if (!qs->generated) {
        for (int k = 0; k < PACER_Q_KIND_COUNT; ++k) {
            glGenQueries(PACER_QUERY_BUFFERS, qs->q[k].ids);
            for (int b = 0; b < PACER_QUERY_BUFFERS; ++b) {
                qs->q[k].ended[b] = false;
            }
            qs->q[k].active = -1;
            qs->q[k].write = 0;
            qs->q[k].last_value = 0;
        }
        qs->generated = true;
    }

    GLuint64 frag = frame_pacer_query_step(&qs->q[PACER_Q_FRAG], GL_FRAGMENT_SHADER_INVOCATIONS_ARB);
    GLuint64 vert = frame_pacer_query_step(&qs->q[PACER_Q_VERT], GL_VERTEX_SHADER_INVOCATIONS_ARB);
    GLuint64 comp = frame_pacer_query_step(&qs->q[PACER_Q_COMP], GL_COMPUTE_SHADER_INVOCATIONS_ARB);

    return (double)frag + g_cfg.alpha * (double)vert + g_cfg.beta * (double)comp;
}

static uint64_t frame_pacer_stream_key(const Thread_Context *ctx)
{
    uint64_t lo = ctx ? (uint64_t)(uint32_t)ctx->thread_id : 0;
    uint64_t hi = ctx ? (uint64_t)(uint32_t)ctx->process_id : 0;
    return (hi << 32) | lo;
}

static uint64_t frame_pacer_get_frame_time(const Thread_Context *ctx) {
    if (ctx->device_id != EXPRESS_GPU_DEVICE_ID) {
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
    if (surface->swap_time_cnt <= 3) {
        return 0;
    }
    int loc = (surface->swap_loc - 1 + 5) % 5;
    return surface->swap_time[loc];
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

static FramePacerState *frame_pacer_get_state_locked(uint64_t key)
{
    FramePacerState *st = (FramePacerState *)g_hash_table_lookup(g_states, (gpointer)(uintptr_t)key);
    if (!st) {
        st = g_new0(FramePacerState, 1);
        st->k_us_per_unit = 0.0;
        g_hash_table_insert(g_states, (gpointer)(uintptr_t)key, st);
    }
    return st;
}

static int frame_pacer_cmp_double(const void *a, const void *b)
{
    double da = *(const double *)a;
    double db = *(const double *)b;
    if (da < db) return -1;
    if (da > db) return 1;
    return 0;
}

/*
 * Robust estimate of the hardware "floor" ratio (time per unit complexity).
 * The raw minimum latches onto a single degenerate frame (near-zero measured
 * time, or a stale/huge complexity) and collapses k, disabling pacing. Instead
 * we take a low percentile over the window, which rejects a handful of
 * transient outliers while still tracking the true uncontended rate. With few
 * samples this naturally degrades to (near) the minimum.
 */
static double frame_pacer_min_ratio(const double ratios[PACER_HISTORY], const gint64 times[PACER_HISTORY],
                                    int count, int head, gint64 now_us, double window_us)
{
    double sorted[PACER_HISTORY];
    int n = 0;
    for (int i = 0; i < count; ++i) {
        int idx = (head - 1 - i + PACER_HISTORY) % PACER_HISTORY;
        if (times[idx] == 0 || now_us - times[idx] > (gint64)window_us) {
            continue;
        }
        double r = ratios[idx];
        if (r <= 0.0 || !isfinite(r)) {
            continue;
        }
        sorted[n++] = r;
    }
    if (n == 0) {
        return 0.0;
    }
    qsort(sorted, n, sizeof(double), frame_pacer_cmp_double);
    int pidx = (int)(PACER_MIN_PERCENTILE * (n - 1));
    if (pidx < 0) {
        pidx = 0;
    }
    if (pidx >= n) {
        pidx = n - 1;
    }
    return sorted[pidx];
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
        g_cfg.alpha = cfg->alpha;
        g_cfg.beta = cfg->beta;
    } else {
        g_cfg.kp = 0.1;
        g_cfg.ki = 0.01;
        g_cfg.window_sec = 1.0;
        g_cfg.custom_offset_us = 0.0;
        g_cfg.alpha = PACER_DEFAULT_ALPHA;
        g_cfg.beta = PACER_DEFAULT_BETA;
    }
    qemu_mutex_unlock(&g_lock);
}

static void frame_pacer_maybe_log_stats_locked(gint64 now_us, double ratio, double sleep_us, double k)
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
    LOGI("frame_pacer: frames=%" PRIu64 " ratio[min/avg/max]=%.9f/%.9f/%.9f sleep_avg_us=%.3f k=%.9f", g_stats.frames, g_stats.ratio_min, avg_ratio, g_stats.ratio_max, avg_sleep, k);

    g_stats.last_log_us = now_us;
    g_stats.ratio_sum = 0.0;
    g_stats.sleep_sum = 0.0;
    g_stats.ratio_min = 0.0;
    g_stats.ratio_max = 0.0;
    g_stats.frames = 0;
}

double frame_pacer_on_frame(uint64_t stream_key, double complexity, uint64_t t_real_us)
{
    if (complexity <= 0.0 || !isfinite(complexity) || t_real_us == 0) {
        return 0.0;
    }

    gint64 now_us = g_get_monotonic_time();
    double sleep_us = 0.0;
    double ratio = t_real_us / complexity;
    if (ratio <= 0.0 || !isfinite(ratio)) {
        return 0.0;
    }
    frame_pacer_stats_sink sink = NULL;

    frame_pacer_init_once();
    qemu_mutex_lock(&g_lock);
    FramePacerState *st = frame_pacer_get_state_locked(stream_key);

    st->times[st->head] = now_us;
    st->ratios[st->head] = ratio;
    st->head = (st->head + 1) % PACER_HISTORY;
    if (st->count < PACER_HISTORY) {
        st->count++;
    }

    double k_us = g_worker_k_cached;
    if (k_us <= 0.0) {
        double window_us = (g_cfg.window_sec > 0.0 ? g_cfg.window_sec : 1.0) * USEC_PER_SEC;
        k_us = frame_pacer_min_ratio(st->ratios, st->times, st->count, st->head, now_us, window_us);
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
    sink = g_stats_sink;
    frame_pacer_maybe_log_stats_locked(now_us, ratio, sleep_us, k_us);
    qemu_mutex_unlock(&g_lock);

    if (sink) {
        LOGD("worker: reporting ratio %.9f", ratio);
        sink(ratio);
    }

    if (sleep_us > 0.0) {
        g_usleep((gulong)sleep_us);
    }
    return sleep_us;
}

void frame_pacer_set_stats_sink(frame_pacer_stats_sink sink)
{
    frame_pacer_init_once();
    qemu_mutex_lock(&g_lock);
    g_stats_sink = sink;
    qemu_mutex_unlock(&g_lock);
}

void frame_pacer_parent_ingest_ratio(double ratio)
{
    if (ratio <= 0.0 || !isfinite(ratio)) {
        return;
    }
    gint64 now_us = g_get_monotonic_time();
    frame_pacer_init_once();
    qemu_mutex_lock(&g_lock);
    g_parent_k.ratios[g_parent_k.head] = ratio;
    g_parent_k.times[g_parent_k.head] = now_us;
    g_parent_k.head = (g_parent_k.head + 1) % PACER_HISTORY;
    if (g_parent_k.count < PACER_HISTORY) {
        g_parent_k.count++;
    }

    double window_us = (g_cfg.window_sec > 0.0 ? g_cfg.window_sec : 1.0) * USEC_PER_SEC;
    g_parent_k.k_us_per_unit = frame_pacer_min_ratio(g_parent_k.ratios, g_parent_k.times,
                                                     g_parent_k.count, g_parent_k.head,
                                                     now_us, window_us);
    qemu_mutex_unlock(&g_lock);
}

double frame_pacer_parent_current_k(void)
{
    frame_pacer_init_once();
    qemu_mutex_lock(&g_lock);
    gint64 now_us = g_get_monotonic_time();
    double window_us = (g_cfg.window_sec > 0.0 ? g_cfg.window_sec : 1.0) * USEC_PER_SEC;
    g_parent_k.k_us_per_unit = frame_pacer_min_ratio(g_parent_k.ratios, g_parent_k.times,
                                                     g_parent_k.count, g_parent_k.head,
                                                     now_us, window_us);
    double k = g_parent_k.k_us_per_unit;
    qemu_mutex_unlock(&g_lock);
    return k;
}

void frame_pacer_worker_set_k(double k)
{
    if (k < 0.0 || !isfinite(k)) {
        return;
    }
    frame_pacer_init_once();
    qemu_mutex_lock(&g_lock);
    LOGD("worker set k: %.9f->%.9f", g_worker_k_cached, k);
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
