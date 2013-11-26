#include <config.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <stdarg.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>

#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/file.h>

#include <ls/daemon.h>

#define DAEMON_ENV_PATH	"/usr/local/sbin:/usr/local/bin:" \
			"/usr/sbin:/usr/bin:/sbin:/bin"
#define DAEMON_ENV_LANG	"C"

static int get_extra_fds(int **pfda, int *pfda_len)
{
	DIR *dir;
	struct dirent entry, *result;
	int rc = -1, fda_len = 0, fda_size = 0, *fda = NULL, fd;

	dir = opendir("/proc/self/fd");
	if (!dir)
		goto err;
	while (1) {
		rc = readdir_r(dir, &entry, &result);
		if (rc > 0) {
			errno = rc;
			rc = -1;
			goto err2;
		}
		if (!result)
			break;
		if (entry.d_name[0] == '.')
			continue;
		fd = atoi(entry.d_name);
		if (fd < 3 || fd == dirfd(dir))
			continue;
		if (fda_len >= fda_size) {
			int *new_fda;

			fda_size *= 2;
			if (fda_size == 0)
				fda_size = 1;
			new_fda = realloc(fda, sizeof(*fda) * fda_size);
			if (!new_fda) {
				free(fda);
				rc = -1;
				goto err2;
			}
			fda = new_fda;
		}
		fda[fda_len++] = fd;
	}
	*pfda = fda;
	*pfda_len = fda_len;
err2:
	closedir(dir);
err:
	return rc;
}

static int close_extra_fds(void)
{
	int *fda, fda_len;

	if (get_extra_fds(&fda, &fda_len)) {
		struct rlimit rlim;
		int max, fd;

		if (getrlimit(RLIMIT_NOFILE, &rlim))
			goto err;
		max = rlim.rlim_max;
		for (fd = 3; fd < max; fd++) {
			if (close(fd) && errno != EINTR && errno != EBADF)
				goto err;
		}
	} else {
		int i;

		for (i = 0; i < fda_len; i++) {
			if (close(fda[i]) && errno != EINTR)
				goto err2;
		}
		free(fda);
	}

	return 0;
err2:
	free(fda);
err:
	return -1;
}

static int save_pid(const char *fmt, va_list ap)
{
	char buf[PATH_MAX];
	int rc, fd, len;

	len = snprintf(buf, sizeof(buf), "%s", "/var/run/");
	if (len < 0 || len >= sizeof(buf)) {
		errno = EINVAL;
		goto err;
	}
	rc = vsnprintf(buf + len, sizeof(buf) - len, fmt, ap);
	if (rc < 0 || rc + len >= sizeof(buf)) {
		errno = EINVAL;
		goto err;
	}
	len += rc;
	rc = snprintf(buf + len, sizeof(buf) - len, "%s", ".pid");
	if (rc < 0 || rc + len >= sizeof(buf)) {
		errno = EINVAL;
		goto err;
	}

	fd = open(buf, O_WRONLY | O_CREAT, 0644);
	if (fd < 0)
		goto err;
	if (flock(fd, LOCK_EX | LOCK_NB))
		goto err2;
	if (ftruncate(fd, 0))
		goto err2;
	len = snprintf(buf, sizeof(buf), "%d\n", getpid());
	if (len < 0 || len >= sizeof(buf)) {
		errno = EINVAL;
		goto err2;
	}
	rc = write(fd, buf, len);
	if (rc != len) {
		if (rc >= 0)
			errno = EIO;
		goto err2;
	}

	return 0;
err2:
	close(fd);
err:
	return -1;
}

static int first_child(int notify_fd, const char *fmt, va_list ap)
{
	int rc, fd, error;

	if (setsid() == (pid_t)-1)
		goto err;

	switch (fork()) {
	case 0:
		break;
	case -1:
		goto err;
	default:
		_exit(EXIT_SUCCESS);
	}

	if (notify_fd < 3) {
		if (dup2(notify_fd, 3) < 0)
			goto err;
		notify_fd = 3;
	}
	fd = open("/dev/null", O_RDONLY);
	if (fd < 0)
		goto err;
	if (fd != STDIN_FILENO) {
		if (dup2(fd, STDIN_FILENO) < 0)
			goto err;
		close(fd);
	}
	fd = open("/dev/null", O_WRONLY);
	if (fd < 0)
		goto err;
	if (fd != STDOUT_FILENO && dup2(fd, STDOUT_FILENO) < 0)
		goto err;
	if (fd != STDERR_FILENO && dup2(fd, STDERR_FILENO) < 0)
		goto err;
	if (fd != STDOUT_FILENO && fd != STDERR_FILENO)
		close(fd);

	umask(0);

	if (chdir("/"))
		goto err;

	rc = save_pid(fmt, ap);
	if (rc)
		goto err;

	return notify_fd;
err:
	error = errno;
	write(notify_fd, &error, sizeof(error));
	_exit(EXIT_FAILURE);
}

void daemon_notify(int notify_fd, int error)
{
	int rc;

	rc = write(notify_fd, &error, sizeof(error));
	if (error || rc < 0)
		_exit(EXIT_FAILURE);
	close(notify_fd);
}

int daemon_change_privilege(const char *user, const char *group, bool nochroot)
{
	struct passwd pwd, *ppwd;
	struct group grp, *pgrp;
	long pwd_buf_size, grp_buf_size;
	char *pwd_buf, *grp_buf = NULL;
	int rc;

	pwd_buf_size = sysconf(_SC_GETPW_R_SIZE_MAX);
	if (pwd_buf_size == -1)
		pwd_buf_size = 16384;
	pwd_buf = malloc(pwd_buf_size);
	if (!pwd_buf)
		goto err;
	rc = getpwnam_r(user, &pwd, pwd_buf, pwd_buf_size, &ppwd);
	if (rc > 0) {
		errno = rc;
		goto err2;
	}
	if (!ppwd) {
		errno = ENOENT;
		goto err2;
	}
	if (!pwd.pw_dir) {
		errno = EINVAL;
		goto err2;
	}

	if (group) {
		grp_buf_size = sysconf(_SC_GETGR_R_SIZE_MAX);
		if (grp_buf_size == -1)
			grp_buf_size = 16384;
		grp_buf = malloc(grp_buf_size);
		if (!grp_buf)
			goto err2;
		rc = getgrnam_r(group, &grp, grp_buf, grp_buf_size, &pgrp);
		if (rc > 0) {
			errno = rc;
			goto err3;
		}
		if (!pgrp) {
			errno = ENOENT;
			goto err3;
		}
		pwd.pw_gid = grp.gr_gid;
	}

	if (!nochroot && (chroot(pwd.pw_dir) || chdir("/")))
		goto err3;

	if (setgroups(1, &pwd.pw_gid) ||
	    setresgid(pwd.pw_gid, pwd.pw_gid, pwd.pw_gid) ||
	    setresuid(pwd.pw_uid, pwd.pw_uid, pwd.pw_uid))
		goto err3;

	if (setenv("USER", user, 1) ||
	    setenv("HOME", nochroot ? pwd.pw_dir : "/", 1))
		goto err3;

	free(grp_buf);
	free(pwd_buf);

	return 0;
err3:
	free(grp_buf);
err2:
	free(pwd_buf);
err:
	return -1;
}

int daemonize(const char *fmt, ...)
{
	sigset_t set;
	int sig, notify_fd[2], rc, error;
	va_list ap;
	struct sigaction act = {
		.sa_handler	= SIG_DFL,
		.sa_flags	= SA_RESTART,
	};

	if (close_extra_fds())
		goto err;

	for (sig = 1; sig < _NSIG; sig++) {
		if (sig == SIGKILL || sig == SIGSTOP)
			continue;
		if (sigaction(sig, &act, NULL) && errno != EINVAL)
			goto err;
	}

	sigemptyset(&set);
	if (sigprocmask(SIG_SETMASK, &set, NULL))
		goto err;

	if (clearenv() || setenv("PATH", DAEMON_ENV_PATH, 1) ||
	    setenv("LANG", DAEMON_ENV_LANG, 1))
		goto err;

	if (pipe(notify_fd))
		goto err;

	switch (fork()) {
	case 0:
		close(notify_fd[0]);
		va_start(ap, fmt);
		rc = first_child(notify_fd[1], fmt, ap);
		va_end(ap);
		return rc;
	case -1:
		goto err3;
	}
	close(notify_fd[1]);
	while (1) {
		rc = read(notify_fd[0], &error, sizeof(error));
		if (rc < 0) {
			if (errno == EINTR)
				continue;
			goto err2;
		}
		break;
	}
	close(notify_fd[0]);
	if (rc != sizeof(error)) {
		errno = EIO;
		goto err;
	}
	if (error) {
		errno = error;
		goto err;
	}
	_exit(EXIT_SUCCESS);
err3:
	close(notify_fd[1]);
err2:
	close(notify_fd[0]);
err:
	return -1;
}
