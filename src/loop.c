#include <ls/loop.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/stat.h>

void add_timer(struct loop *loop, struct timer *timer)
{
	struct rb_node **p;
	struct rb_node *parent = NULL;
	struct timer *ptr;

	assert(loop);
	assert(timer);
	assert(RB_EMPTY_NODE(&timer->node));
	assert(timer->callback);

	p = &loop->root.rb_node;
	while (*p) {
		parent = *p;
		ptr = rb_entry(parent, struct timer, node);
		if (timercmp(&timer->expires, &ptr->expires, <))
			p = &parent->rb_left;
		else
			p = &parent->rb_right;
	}
	rb_link_node(&timer->node, parent, p);
	rb_insert_color(&timer->node, &loop->root);

	if(!loop->next || timercmp(&timer->expires, &loop->next->expires, <))
		loop->next = timer;
}

void add_timeout(struct loop *loop, struct timer *timer, unsigned int ms)
{
	struct timeval delta;

	assert(loop);
	assert(timer);

	delta.tv_sec = ms / 1000;
	delta.tv_usec = (ms % 1000) * 1000;
	timeradd(&loop->realtime, &delta, &timer->expires);

	add_timer(loop, timer);
}

void del_timer(struct loop *loop, struct timer *timer)
{
	assert(loop);
	assert(timer);

	if (RB_EMPTY_NODE(&timer->node))
		return;

	if (loop->next == timer) {
		struct rb_node *rbn = rb_next(&timer->node);

		loop->next = rbn ? rb_entry(rbn, struct timer, node) : NULL;
	}
	rb_erase(&timer->node, &loop->root);

	RB_CLEAR_NODE(&timer->node);
}

static int __run_timers(struct loop *loop)
{
	struct timer *timer;

	assert(loop);

	gettimeofday(&loop->realtime, NULL);
	while (loop->next) {
		if (timercmp(&loop->realtime, &loop->next->expires, <)) {
			struct timeval delta;
			int ms;

			timersub(&loop->next->expires, &loop->realtime, &delta);
			ms = delta.tv_sec * 1000 + delta.tv_usec / 1000;
			/**
			 * The time resolution supported by epoll_wait(2) is
			 * millisecond, so set it to 1 millisecond to prevent
			 * useless wakeup.
			 */
			if (ms == 0)
				ms = 1;

			return ms;
		}
		timer = loop->next;
		del_timer(loop, loop->next);
		timer->callback(loop, timer);
		if (loop->stopped)
			break;
	}

	return -1;
}

int add_io(struct loop *loop, int fd, enum IO event, struct io *io)
{
	int op;
	struct epoll_event eev;

	assert(loop);
	assert(fd >= 0);
	assert(event == IO_READ || event == IO_WRITE);
	assert(io);
	assert(io->callback);

	if (fd >= loop->fd_size) {
		size_t new_size = loop->fd_size * 2;
		struct io_fd *new_fd;

		new_fd = realloc(loop->fd, new_size * sizeof(*loop->fd));
		if (!new_fd)
			goto err;
		memset(new_fd + loop->fd_size, 0,
			loop->fd_size * sizeof(*loop->fd));
		loop->fd = new_fd;
		loop->fd_size = new_size;
	}

	assert(!loop->fd[fd].io[event]);
	if (loop->fd[fd].io[!event]) {
		op = EPOLL_CTL_MOD;
		eev.events = EPOLLIN | EPOLLOUT;
	} else {
		op = EPOLL_CTL_ADD;
		eev.events = event == IO_READ ? EPOLLIN : EPOLLOUT;
	}
	eev.data.fd = fd;
	if (epoll_ctl(loop->ep_fd, op, fd, &eev) < 0)
		goto err;
	loop->fd[fd].io[event] = io;
	if (op == EPOLL_CTL_ADD)
		loop->activated++;

	return 0;
err:
	return -1;
}

void del_io(struct loop *loop, int fd, enum IO event)
{
	assert(loop);
	assert(fd >= 0);
	assert(event == IO_READ || event == IO_WRITE);

	if (fd < loop->fd_size && loop->fd[fd].io[event]) {
		int op;
		struct epoll_event eev;

		if (loop->fd[fd].io[!event]) {
			op = EPOLL_CTL_MOD;
			eev.events = event == IO_READ ? EPOLLOUT : EPOLLIN;
			eev.data.fd = fd;
		} else {
			op = EPOLL_CTL_DEL;
		}
		if (epoll_ctl(loop->ep_fd, op, fd, &eev))
			abort();
		loop->fd[fd].io[event] = NULL;
		if (op == EPOLL_CTL_DEL)
			loop->activated--;
	}
}

void close_io(struct loop *loop, int fd)
{
	assert(loop);
	assert(fd >= 0);
	
	if (fd < loop->fd_size &&
	    (loop->fd[fd].io[IO_READ] || loop->fd[fd].io[IO_WRITE])) {
		loop->fd[fd].io[IO_READ] = NULL;
		loop->fd[fd].io[IO_WRITE] = NULL;
		loop->activated--;
	}
	/**
	 * Assume the fd isn't duplicated explicitly or implicitly(via fork(2)),
	 * and close(2) will be called after this. This assumption can save
	 * an epoll_ctl(2) in some case.
	 */
}

int add_ilc(struct loop *loop, struct ilc *ilc)
{
	assert(loop);
	assert(ilc);
	assert(ilc->callback);

	if (write(loop->ilc_fd, &ilc, sizeof(ilc)) != sizeof(ilc))
		return -1;

	return 0;
}

void run_loop(struct loop *loop)
{
	int eev_num = 0, timeout, i;

	assert(loop);

	while (!loop->stopped) {
		timeout = __run_timers(loop);
		if (loop->stopped)
			break;
		for (i = 0; i < eev_num; i++) {
			uint32_t events;
			int fd;
			struct io *io;

			events = loop->eev[i].events;
			if (events & (EPOLLHUP | EPOLLERR))
				events |= EPOLLIN | EPOLLOUT;
			fd = loop->eev[i].data.fd;
			if ((events & EPOLLOUT) &&
			    (io = loop->fd[fd].io[IO_WRITE])) {
				io->callback(loop, fd, io);
				if (loop->stopped)
					goto out;
			}

			if ((events & EPOLLIN) &&
			    (io = loop->fd[fd].io[IO_READ])) {
				io->callback(loop, fd, io);
				if (loop->stopped)
					goto out;
			}
		}
		if (eev_num == loop->eev_size) {
			size_t new_size = loop->eev_size * 2;
			struct epoll_event *new_eev;

			new_eev= realloc(loop->eev,
					 new_size * sizeof(*loop->eev));
			if (new_eev) {
				loop->eev = new_eev;
				loop->eev_size = new_size;
			}
		}
		timeout = __run_timers(loop);
		if (loop->stopped)
			break;
		eev_num = epoll_wait(loop->ep_fd, loop->eev, loop->eev_size,
				     timeout);
	}
out:
	/* Fire all the ILC events, otherwise memory may leak. */
	loop->ilc_io.callback(loop, loop->ilc_fd, &loop->ilc_io);

	return;
}

static int __tmpfifo(void)
{
	char dir[] = "/dev/shm/tmpfifo-XXXXXX";
	char path[PATH_MAX];
	int fd, retval;

	if (!mkdtemp(dir))
		goto err;
	retval = snprintf(path, sizeof(path), "%s/0", dir);
	if (retval < 0 || retval >= sizeof(path))
		goto err2;
	if (mkfifo(path, 0600))
		goto err2;
#ifdef O_CLOEXEC
	fd = open(path,  O_RDWR | O_CLOEXEC | O_NONBLOCK);
	if (fd < 0)
		goto err3;
#else
	fd = open(path,, O_RDWR | O_NONBLOCK);
	if (fd < 0)
		goto err3;
	if (fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC) == -1) {
		close(fd);
		goto err3;
	}
#endif
	unlink(path);
	rmdir(dir);

	return fd;
err3:
	unlink(path);
err2:
	rmdir(dir);
err:
	return -1;
}

static void __ilc_callback(struct loop *loop, int fd, struct io *io)
{
	ssize_t len;

	len = read(fd, loop->ilc, loop->ilc_size);
	if (len > 0) {
		size_t n = len / sizeof(*loop->ilc);
		size_t i;

		for (i = 0; i < n; i++)
			loop->ilc[i]->callback(loop, loop->ilc[i]);
		if (len == loop->ilc_size) {
			struct ilc **new_ilc;
			size_t new_size = loop->ilc_size * 2;

			new_ilc = realloc(loop->ilc, new_size);
			if (new_ilc) {
				loop->ilc = new_ilc;
				loop->ilc_size = new_size;
			}
		}
	}
}

struct loop *alloc_loop(size_t size)
{
	struct loop *loop;
       
	assert(size > 0);

	loop = malloc(sizeof(*loop));
	if (!loop)
		goto err;

	loop->stopped = false;
#ifdef EPOLL_CLOEXEC
	loop->ep_fd = epoll_create1(EPOLL_CLOEXEC);
	if (loop->ep_fd < 0)
		goto err2;
#else
	loop->ep_fd = epoll_create(1);
	if (loop->ep_fd < 0)
		goto err2;
	if (fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC) == -1)
		goto err3;
#endif
	loop->activated = 0;
	loop->fd = calloc(size, sizeof(*loop->fd));
	if (!loop->fd)
		goto err3;
	loop->fd_size = size;
	loop->eev = calloc(size, sizeof(*loop->eev));
	if (!loop->eev)
		goto err4;
	loop->eev_size = size;

	if (gettimeofday(&loop->realtime, NULL))
		goto err5;
	loop->root = RB_ROOT;
	loop->next = NULL;

	loop->ilc_fd = __tmpfifo();
	if (loop->ilc_fd < 0)
		goto err5;
	loop->ilc_size = sysconf(_SC_PAGESIZE);
	loop->ilc = malloc(loop->ilc_size);
	if (!loop->ilc)
		goto err6;
	loop->ilc_io.callback = __ilc_callback;
	if (add_io(loop, loop->ilc_fd, IO_READ, &loop->ilc_io))
		goto err7;

	return loop;
err7:
	free(loop->ilc);
err6:
	close(loop->ilc_fd);
err5:
	free(loop->eev);
err4:
	free(loop->fd);
err3:
	close(loop->ep_fd);
err2:
	free(loop);
err:
	return NULL;
}

void free_loop(struct loop *loop)
{
	if (loop) {
		int fd;
#ifndef NDEBUG
		struct ilc *ilc;

		/* Make sure no pending ILC, otherwise memory may leak. */
		assert(read(loop->ilc_fd, &ilc, sizeof(ilc)) < 0);
#endif
		del_io(loop, loop->ilc_fd, IO_READ);

		/* All the fds should be closed */
		assert(!loop->activated);
		for (fd = 0; fd < loop->fd_size; fd++) {
			assert(!loop->fd[fd].io[IO_READ]);
			assert(!loop->fd[fd].io[IO_WRITE]);
		}

		/* No timer is activated */
		assert(RB_EMPTY_ROOT(&loop->root));
		assert(!loop->next);

		free(loop->ilc);
		close(loop->ilc_fd);
		free(loop->eev);
		free(loop->fd);
		close(loop->ep_fd);
		free(loop);
	}
}

void stop_loop(struct loop *loop)
{
	loop->stopped = true;
}

static void __stop_remote_ilc_callback(struct loop *loop, struct ilc *ilc)
{
	stop_loop(loop);
}

static struct ilc __stop_remote_ilc = {
	.callback	= __stop_remote_ilc_callback
};

int stop_remote_loop(struct loop *loop)
{
	return add_ilc(loop, &__stop_remote_ilc);
}
