#ifndef __LS_STRING_H
#define __LS_STRING_H

#include <stdarg.h>
#include <stdlib.h>

#include <sys/types.h>

int vscnprintf(char *buf, size_t size, const char *fmt, va_list args);
int scnprintf(char *buf, size_t size, const char *fmt, ...);

char *strlstrip(char *str);
char *strrstrip(char *str);
char *strstrip(char *str);

char *astrncat(char *dest, const char *src, unsigned int src_len);
char *astrcat(char *dest, const char *src);

struct strv {
	char		**strv;
	unsigned int	len, size;
};

static inline void strv_init(struct strv *sv)
{
	sv->strv = NULL;
	sv->len = sv->size = 0;
}

void strv_free(struct strv *sv);
int strv_append3(struct strv *sv, const char *str, int len);
int strv_append(struct strv *sv, const char *str);

#endif /* __LS_STRING_H */
