#ifndef __LS_PR_H
#define __LS_PR_H

#include <stdio.h>
#include <systemd/sd-daemon.h>

static inline __attribute__((format(printf, 1, 2)))
int no_pr(const char *fmt, ...)
{
	return 0;
}

#define pr(fmt, args...) \
	fprintf(stderr, fmt, ##args)

#define pr_emerg(fmt, args...) \
	pr(SD_EMERG fmt, ##args)
#define pr_alert(fmt, args...) \
	pr(SD_ALERT fmt, ##args)
#define pr_crit(fmt, args...) \
	pr(SD_CRIT fmt, ##args)
#define pr_err(fmt, args...) \
	pr(SD_ERR fmt, ##args)
#define pr_warning(fmt, args...) \
	pr(SD_WARNING fmt, ##args)
#define pr_warn pr_warning
#define pr_notice(fmt, args...) \
	pr(SD_NOTICE fmt, ##args)
#define pr_info(fmt, args...) \
	pr(SD_INFO fmt, ##args)
#ifdef NDEBUG
#define pr_debug(fmt, args...) \
	no_pr(SD_DEBUG fmt, ##args)
#else
#define pr_debug(fmt, args...) \
	pr(SD_DEBUG fmt, ##args)
#endif

#endif /* __LS_PR_H */
