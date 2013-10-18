#ifndef __LS_STRING_H
#define __LS_STRING_H

#include <stdarg.h>

#include <sys/types.h>

int vscnprintf(char *buf, size_t size, const char *fmt, va_list args);
int scnprintf(char *buf, size_t size, const char *fmt, ...);

#endif /* __LS_STRING_H */
