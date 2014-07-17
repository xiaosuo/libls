#ifndef __LS_PR_H
#define __LS_PR_H

#include <stdio.h>
#include <stdlib.h>

static inline __attribute__((format(printf, 1, 2)))
int no_pr(const char *fmt, ...)
{
	return 0;
}

__attribute__((format(printf, 1, 2)))
extern int (*pr)(const char *fmt, ...);

#define LS_EMERG   "<0>"  /* system is unusable */
#define LS_ALERT   "<1>"  /* action must be taken immediately */
#define LS_CRIT    "<2>"  /* critical conditions */
#define LS_ERR     "<3>"  /* error conditions */
#define LS_WARNING "<4>"  /* warning conditions */
#define LS_NOTICE  "<5>"  /* normal but significant condition */
#define LS_INFO    "<6>"  /* informational */
#define LS_DEBUG   "<7>"  /* debug-level messages */

#define pr_emerg(fmt, args...) \
	pr(LS_EMERG fmt, ##args)
#define pr_alert(fmt, args...) \
	pr(LS_ALERT fmt, ##args)
#define pr_crit(fmt, args...) \
	pr(LS_CRIT fmt, ##args)
#define pr_err(fmt, args...) \
	pr(LS_ERR fmt, ##args)
#define pr_warning(fmt, args...) \
	pr(LS_WARNING fmt, ##args)
#define pr_warn pr_warning
#define pr_notice(fmt, args...) \
	pr(LS_NOTICE fmt, ##args)
#define pr_info(fmt, args...) \
	pr(LS_INFO fmt, ##args)
#ifdef NDEBUG
#define pr_debug(fmt, args...) \
	no_pr(LS_DEBUG fmt, ##args)
#else
#define pr_debug(fmt, args...) \
	pr(LS_DEBUG fmt, ##args)
#endif

#define die(fmt, args...) \
	do { \
		pr_err(fmt "\n", ##args); \
		exit(EXIT_FAILURE); \
	} while (0)

#define fail(fmt, args...) \
	die("Failed to " fmt, ##args)

#endif /* __LS_PR_H */
