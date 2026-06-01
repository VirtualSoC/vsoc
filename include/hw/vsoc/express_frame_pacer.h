#ifndef EXPRESS_FRAME_PACER_H
#define EXPRESS_FRAME_PACER_H

#include <stdbool.h>
#include <stdint.h>

struct Thread_Context;

typedef struct FramePacerConfig {
    double kp;
    double ki;
    double window_sec;
    double custom_offset_us;
} FramePacerConfig;

/*
 * Record a frame boundary for a rendering stream. The pacer applies any
 * computed sleep internally and returns the sleep duration in microseconds.
 * `stream_key` should uniquely identify the producer (e.g., process+thread id).
 * `complexity` defaults to 1.0 if non-positive and is used to normalize timing.
 * `frame_time_us` is the measured frame interval in microseconds.
 */
double frame_pacer_on_frame(uint64_t stream_key, double complexity, uint64_t t_real_us);

/*
 * Update global PI gains/window/offset. Passing NULL restores defaults.
 */
void frame_pacer_configure(const FramePacerConfig *cfg);

void frame_pacer_maybe_apply(struct Thread_Context *context, uint64_t id);

// Global K ownership lives in parent; workers cache the latest pushed value.
void frame_pacer_parent_ingest_ratio(double ratio);
double frame_pacer_parent_current_k(void);
void frame_pacer_worker_set_k(double k);
double frame_pacer_worker_get_k(void);

typedef void (*frame_pacer_stats_sink)(double ratio);
void frame_pacer_set_stats_sink(frame_pacer_stats_sink sink);

#endif /* EXPRESS_FRAME_PACER_H */
