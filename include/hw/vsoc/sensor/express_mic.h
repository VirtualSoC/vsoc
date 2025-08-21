#ifndef EXPRESS_MICROPHONE_H
#define EXPRESS_MICROPHONE_H


void express_mic_status_changed(const void *buf, int size);

void sync_express_mic_status(void);

int start_capture_from_file(char *path);

int start_capture(void);

void stop_capture(void);
#endif