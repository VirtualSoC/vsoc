#ifndef QEMU_EXPRESS_LOG_H
#define QEMU_EXPRESS_LOG_H


#ifdef STD_DEBUG_LOG
#define express_printf printf
#else
#define express_printf null_printf
#endif

char *get_now_time(void);
int null_printf(const char *a,...);
#endif

