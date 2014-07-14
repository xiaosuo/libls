#ifndef __LS_EVT_H
#define __LS_EVT_H

#include <sys/time.h>
#include <ls.h>

#include <sys/epoll.h>

struct evt_loop;

struct evt {
	void	(*callback)(struct evt_loop *loop, int fd, struct evt *ev);
};

enum EVT {
	EVT_READ = 0,
	EVT_WRITE,
	EVT_SIZE
};

struct evt_fd {
	struct evt	*evt[EVT_SIZE];
};

/* Inter-Loop Communication */
struct ilc {
	void	(*callback)(struct evt_loop *loop, struct ilc *ilc);
};

struct evt_loop {
	bool			stopped;
	int			ep_fd;
	size_t			activated;
	struct evt_fd		*fd;
	size_t			fd_size;
	struct epoll_event	*eev;
	size_t			eev_size;

	/* For timers */
	struct timeval		realtime;
	struct rb_root		root;
	struct timer		*next;

	/* For ILC */
	int			ilc_fd;
	struct evt		ilc_evt;
	struct ilc		**ilc;
	size_t			ilc_size;
};

int add_evt(struct evt_loop *loop, int fd, enum EVT event, struct evt *ev);
void del_evt(struct evt_loop *loop, int fd, enum EVT event);
void close_evt(struct evt_loop *loop, int fd);

struct timer {
	struct timeval	expires;
	struct rb_node	node;
	void		(*callback)(struct evt_loop *loop, struct timer *timer);
};

static inline void init_timer(struct timer *timer)
{
	assert(timer);

	RB_CLEAR_NODE(&timer->node);
}
void add_timer(struct evt_loop *loop, struct timer *timer);
void add_timeout(struct evt_loop *loop, struct timer *timer, unsigned int ms);
void del_timer(struct evt_loop *loop, struct timer *timer);
static inline void update_timeout(struct evt_loop *loop, struct timer *timer,
		unsigned int ms)
{
	del_timer(loop, timer);
	add_timeout(loop, timer, ms);
}

int add_ilc(struct evt_loop *loop, struct ilc *ilc);

struct evt_loop *alloc_evt_loop(size_t size);
void free_evt_loop(struct evt_loop *loop);
void run_evt_loop(struct evt_loop *loop);
void stop_evt_loop(struct evt_loop *loop);

int stop_remote_evt_loop(struct evt_loop *loop);

#endif /* __LS_EVT_H */
