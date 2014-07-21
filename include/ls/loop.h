#ifndef __LS_LOOP_H
#define __LS_LOOP_H

#include <sys/time.h>
#include <ls.h>

#include <sys/epoll.h>

struct loop;

struct io {
	void	(*callback)(struct loop *loop, int fd, struct io *io);
};

enum IO {
	IO_READ = 0,
	IO_WRITE,
	IO_SIZE
};

struct io_fd {
	struct io	*io[IO_SIZE];
};

/* Inter-Loop Communication */
struct ilc {
	void	(*callback)(struct loop *loop, struct ilc *ilc);
};

struct loop {
	bool			stopped;
	int			ep_fd;
	size_t			activated;
	struct io_fd		*fd;
	size_t			fd_size;
	struct epoll_event	*eev;
	size_t			eev_size;

	/* For timers */
	struct timeval		realtime;
	struct rb_root		root;
	struct timer		*next;

	/* For ILC */
	int			ilc_fd;
	struct io		ilc_io;
	struct ilc		**ilc;
	size_t			ilc_size;
};

int add_io(struct loop *loop, int fd, enum IO event, struct io *io);
static inline int add_i(struct loop *loop, int fd, struct io *io)
{
	return add_io(loop, fd, IO_READ, io);
}
static inline int add_o(struct loop *loop, int fd, struct io *io)
{
	return add_io(loop, fd, IO_WRITE, io);
}
void del_io(struct loop *loop, int fd, enum IO event);
static inline void del_i(struct loop *loop, int fd)
{
	del_io(loop, fd, IO_READ);
}
static inline void del_o(struct loop *loop, int fd)
{
	del_io(loop, fd, IO_WRITE);
}
void close_io(struct loop *loop, int fd);

struct timer {
	struct timeval	expires;
	struct rb_node	node;
	void		(*callback)(struct loop *loop, struct timer *timer);
};

static inline void init_timer(struct timer *timer)
{
	assert(timer);

	RB_CLEAR_NODE(&timer->node);
}
void add_timer(struct loop *loop, struct timer *timer);
void add_timeout(struct loop *loop, struct timer *timer, unsigned int ms);
void del_timer(struct loop *loop, struct timer *timer);
static inline void update_timeout(struct loop *loop, struct timer *timer,
		unsigned int ms)
{
	del_timer(loop, timer);
	add_timeout(loop, timer, ms);
}

int add_ilc(struct loop *loop, struct ilc *ilc);

struct loop *alloc_loop(size_t size);
void free_loop(struct loop *loop);
void run_loop(struct loop *loop);
void stop_loop(struct loop *loop);

int stop_remote_loop(struct loop *loop);

#endif /* __LS_LOOP_H */
