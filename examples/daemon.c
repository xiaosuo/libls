#include <ls.h>
#include <unistd.h>
#include <errno.h>

static void my_puts(const char *str)
{
	if (str) {
		int len = strlen(str);

		write(1, str, len);
	}
}

int main(int argc, char *argv[])
{
	int notify_fd;

	notify_fd = daemonize("test");
	if (notify_fd < 0) {
		perror("Failed to create daemon");
		return EXIT_FAILURE;
	}
	if (daemon_change_privilege("nobody", "nobody", false))
		daemon_notify(notify_fd, errno);
	daemon_notify(notify_fd, 0);
	while (1) {
		extern char **environ;
		int i;

		for (i = 0; environ[i]; i++)
			my_puts(environ[i]);
		my_puts(getenv("LANG"));
		my_puts(getenv("PATH"));
		my_puts(getenv("USER"));
		my_puts(getenv("HOME"));
		my_puts(getenv("PWD"));
		sleep(60);
	}

	return EXIT_SUCCESS;
}
