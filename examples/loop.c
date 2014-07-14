#include <ls.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include <netinet/in.h>

struct server {
	int		sock;
	struct evt_loop	*loop;
	struct evt_loop	*conn_loop;
	struct evt	evt;
	struct timer	timer;
	pthread_t	conn_tid;
	struct list_head	conn_list;
};

struct dist_req {
	struct ilc	ilc;
	int		sock;
	struct server	*serv;
};

struct conn {
	int		sock;
	struct evt	evt;
	struct list_head	link;
};

void set_nonblocking(int fd)
{
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

void read_data(struct evt_loop *loop, int fd, struct evt *ev)
{
	char buf[2048];
	ssize_t len = read(fd, buf, sizeof(buf));

	if (len > 0) {
		if (write(fd, buf, len) != len)
			pr_warn("some data is missing\n");
	} else if (len == 0) {
		struct conn *conn = container_of(ev, struct conn, evt);

		del_evt(loop, fd, EVT_READ);
		close(fd);
		list_del(&conn->link);
		free(conn);
	}
}

void ilc_callback(struct evt_loop *loop, struct ilc *ilc)
{
	struct dist_req *req = container_of(ilc, struct dist_req, ilc);
	struct conn *conn = malloc(sizeof(*conn));

	if (!conn)
		goto err;
	conn->sock = req->sock;
	conn->evt.callback = read_data;
	if (add_evt(loop, conn->sock, EVT_READ, &conn->evt))
		goto err2;
	list_add_tail(&conn->link, &req->serv->conn_list);
	free(req);

	return;
err2:
	free(conn);
err:
	close(req->sock);
	free(req);
}

void dist_conn(struct evt_loop *loop, int fd, struct evt *ev)
{
	int sock = accept(fd, NULL, NULL);
	struct dist_req *req;
	struct server *serv = container_of(ev, struct server, evt);

	if (sock < 0)
		goto err;
	set_nonblocking(sock);
	req = malloc(sizeof(*req));
	if (!req)
		goto err2;
	req->sock = sock;
	req->ilc.callback = ilc_callback;
	req->serv = serv;

	if (add_ilc(serv->conn_loop, &req->ilc))
		goto err3;

	return;
err3:
	free(req);
err2:
	close(sock);
err:
	return;
}

void *conn_thread(void *args)
{
	struct evt_loop *loop = args;

	run_evt_loop(loop);

	return NULL;
}

void stop_service(struct evt_loop *loop, struct timer *timer)
{
	struct server *serv = container_of(timer, struct server, timer);

	stop_remote_evt_loop(serv->conn_loop);
	stop_evt_loop(loop);
}

int main()
{
	struct server serv;
	struct sockaddr_in addr;
	int ok = 1;
	struct conn *conn, *next;

	serv.sock = socket(PF_INET, SOCK_STREAM, 0);
	if (serv.sock < 0) {
		pr_err("Failed to create a socket\n");
		return 1;
	}
	set_nonblocking(serv.sock);
	if (setsockopt(serv.sock, SOL_SOCKET, SO_REUSEADDR, &ok, sizeof(ok))) {
		pr_err("Failed to reuse the address\n");
		return 1;
	}
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(7);
	if (bind(serv.sock, (struct sockaddr *)&addr, sizeof(addr))) {
		pr_err("Failed to bind to the echo service\n");
		return 1;
	}
	if (listen(serv.sock, 10)) {
		pr_err("Failed to listen\n");
		return 1;
	}

	serv.loop = alloc_evt_loop(10);
	if (!serv.loop) {
		pr_err("Failed to allocate the event loop for the server\n");
		return 1;
	}
	serv.conn_loop = alloc_evt_loop(10);
	if (!serv.conn_loop) {
		pr_err("Failed to allocate the event loop for connections\n");
		return 1;
	}
	INIT_LIST_HEAD(&serv.conn_list);
	if (pthread_create(&serv.conn_tid, NULL, conn_thread, serv.conn_loop)) {
		pr_err("Failed to create a separated thread for connections\n");
		return 1;
	}
	serv.evt.callback = dist_conn;
	if (add_evt(serv.loop, serv.sock, EVT_READ, &serv.evt)) {
		pr_err("Failed to monitor the read event of the server\n");
		return 1;
	}

	init_timer(&serv.timer);
	serv.timer.callback = stop_service;
	add_timeout(serv.loop, &serv.timer, 60000);
	run_evt_loop(serv.loop);
	pthread_join(serv.conn_tid, NULL);
	list_for_each_entry_safe(conn, next, &serv.conn_list, link) {
		close_evt(serv.conn_loop, conn->sock);
		close(conn->sock);
		list_del(&conn->link);
	}
	del_evt(serv.loop, serv.sock, EVT_READ);
	close(serv.sock);
	free_evt_loop(serv.conn_loop);
	free_evt_loop(serv.loop);

	return 0;
}
