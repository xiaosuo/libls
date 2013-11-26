#ifndef __LS_DAEMON_H
#define __LS_DAEMON_H

#include <stdbool.h>

int daemonize(const char *fmt, ...);
int daemon_change_privilege(const char *user, const char *group, bool nochroot);
void daemon_notify(int notify_fd, int error);

#endif /* __LS_DAEMON_H */
