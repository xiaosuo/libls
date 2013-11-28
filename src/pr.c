#include <stdarg.h>

#include <ls/pr.h>

static int sd_pr(const char *fmt, ...);

__attribute__((format(printf, 1, 2)))
int (*pr)(const char *fmt, ...) = sd_pr;

static int sd_pr(const char *fmt, ...)
{
	va_list ap;
	int rc;

	va_start(ap, fmt);
	rc = vfprintf(stderr, fmt, ap);
	va_end(ap);

	return rc;
}
