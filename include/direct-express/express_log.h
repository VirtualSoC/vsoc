#ifndef QEMU_EXPRESS_LOG_H
#define QEMU_EXPRESS_LOG_H


#ifdef STD_DEBUG_LOG
#define express_printf printf
#else
#define express_printf null_printf
#endif


#define RED(a) "\033[31m"a"\033[0m"
#define GREEN(a) "\033[32m"a"\033[0m"
#define YELLOW(a) "\033[33m"a"\033[0m"



char *get_now_time(void);
int null_printf(const char *a,...);
#endif

