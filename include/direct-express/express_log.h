#ifndef QEMU_EXPRESS_LOG_H
#define QEMU_EXPRESS_LOG_H


#ifdef STD_DEBUG_LOG
#define express_printf printf
#else
#define express_printf null_printf
#endif

int null_printf(const char *a,...);
char *get_now_time();
#endif

