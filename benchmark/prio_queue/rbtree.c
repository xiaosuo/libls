#include <ls.h>
#include <stdlib.h>
#include <time.h>

#include <sys/time.h>

struct num_node {
	int		num;
	struct rb_node	node;
};

static inline int num_cmp(const struct rb_node *node,
		const struct rb_node *node2)
{
	struct num_node *nn, *nn2;

	nn = rb_entry(node, struct num_node, node);
	nn2 = rb_entry(node2, struct num_node, node);

	if (nn->num < nn2->num)
		return -1;
	else if (nn->num > nn2->num)
		return 1;
	else
		return 0;
}

static inline void my_rb_insert(struct rb_node *node, struct rb_root *root)
{
	struct rb_node **new = &(root->rb_node), *parent = NULL;
	int rc;

	while (*new) {
		parent = *new;
		rc = num_cmp(node, parent);
		if (rc <= 0)
			new = &parent->rb_right;
		else
			new = &parent->rb_left;
	}

	rb_link_node(node, parent, new);
	rb_insert_color(node, root);
}

int main(int argc, char *argv[])
{
	struct rb_root root = { NULL };
	int i, last, n = atoi(argv[1]);
	struct num_node *nn;
	struct timeval start, end, delta;
	struct rb_node *node, *next;

	srand(time(NULL));
	gettimeofday(&start, NULL);
	for (i = 1; i < n; i++) {
		nn = malloc(sizeof(*nn));
		if (!nn)
			break;
		nn->num = rand();
		my_rb_insert(&nn->node, &root);
	}
	gettimeofday(&end, NULL);
	printf("num: %d\n", i);
	timersub(&end, &start, &delta);
	printf("time: %ld.%06ld\n", delta.tv_sec, delta.tv_usec);

	node = rb_first(&root);
	last = rb_entry(node, struct num_node, node)->num;
	i = 0;
	gettimeofday(&start, NULL);
	while (node) {
		next = rb_next(node);
		nn = rb_entry(node, struct num_node, node);
		if (last < nn->num)
			printf("fault\n");
		last = nn->num;
		rb_erase(node, &root);
		free(nn);
		++i;
		node = next;
	}
	gettimeofday(&end, NULL);
	if (i != n - 1)
		printf("fault\n");
	timersub(&end, &start, &delta);
	printf("time: %ld.%06ld\n", delta.tv_sec, delta.tv_usec);

	return 0;
}
