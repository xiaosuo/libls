#include <ls.h>
#include <stdlib.h>
#include <time.h>

#include <sys/time.h>

struct num_node {
	int		num;
	struct avl_node	node;
};

static inline int num_cmp(const struct avl_node *node,
		const struct avl_node *node2)
{
	struct num_node *nn, *nn2;

	nn = avl_entry(node, struct num_node, node);
	nn2 = avl_entry(node2, struct num_node, node);

	if (nn->num < nn2->num)
		return -1;
	else if (nn->num > nn2->num)
		return 1;
	else
		return 0;
}

static inline void avl_insert(struct avl_node *node, struct avl_root *root)
{
	struct avl_node **new = &(root->node), *parent = NULL;
	int rc;

	while (*new) {
		parent = *new;
		rc = num_cmp(node, parent);
		if (rc <= 0)
			new = &parent->right;
		else
			new = &parent->left;
	}

	avl_link_node(node, parent, new);
	avl_insert_balance(node, root);
}

int main(int argc, char *argv[])
{
	struct avl_root root = { NULL };
	int i, last, n = atoi(argv[1]);
	struct num_node *nn;
	struct timeval start, end, delta;
	struct avl_node *node, *next;

	srand(time(NULL));
	gettimeofday(&start, NULL);
	for (i = 1; i < n; i++) {
		nn = malloc(sizeof(*nn));
		if (!nn)
			break;
		nn->num = rand();
		avl_insert(&nn->node, &root);
	}
	gettimeofday(&end, NULL);
	printf("num: %d\n", i);
	timersub(&end, &start, &delta);
	printf("time: %ld.%06ld\n", delta.tv_sec, delta.tv_usec);

	node = avl_first(&root);
	last = avl_entry(node, struct num_node, node)->num;
	i = 0;
	gettimeofday(&start, NULL);
	while (node) {
		next = avl_next(node);
		nn = avl_entry(node, struct num_node, node);
		if (last < nn->num)
			printf("fault\n");
		last = nn->num;
		avl_erase(node, &root);
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
