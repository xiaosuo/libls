#ifndef __LS_STRING_H
#define __LS_STRING_H

#include <stdarg.h>

#include <sys/types.h>

int vscnprintf(char *buf, size_t size, const char *fmt, va_list args);
int scnprintf(char *buf, size_t size, const char *fmt, ...);

char *strlstrip(char *str);
char *strrstrip(char *str);
char *strstrip(char *str);

#endif /* __LS_STRING_H */
