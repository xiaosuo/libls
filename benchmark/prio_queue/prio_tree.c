#include <ls.h>
#include <stdlib.h>
#include <time.h>

#include <sys/time.h>

struct num_node {
	int			num;
	struct prio_node	node;
};

static inline int num_cmp(const struct prio_node *node,
		const struct prio_node *node2)
{
	struct num_node *nn, *nn2;

	nn = prio_entry(node, struct num_node, node);
	nn2 = prio_entry(node2, struct num_node, node);

	if (nn->num < nn2->num)
		return -1;
	else if (nn->num > nn2->num)
		return 1;
	else
		return 0;
}

static inline int get_height(const struct prio_root *root)
{
	struct prio_node *node = root->node;
	int depth = 0;

	while (node) {
		depth++;
		node = node->left;
	}

	return depth;
}

int main(int argc, char *argv[])
{
	struct prio_root root = { NULL };
	int i, last, n = atoi(argv[1]);
	struct num_node *nn;
	struct timeval start, end, delta;
	struct prio_node *node;
	int height;

	srand(time(NULL));
	gettimeofday(&start, NULL);
	for (i = 1; i < n; i++) {
		nn = malloc(sizeof(*nn));
		if (!nn)
			break;
		nn->num = rand();
		prio_insert(&nn->node, &root, num_cmp);
	}
	gettimeofday(&end, NULL);
	printf("num: %d\n", i);
	timersub(&end, &start, &delta);
	printf("time: %ld.%06ld\n", delta.tv_sec, delta.tv_usec);

	height = get_height(&root);
	if (height > 0)
		assert((n - 1) >= (1 << (height - 1)));
	assert((n - 1) < (1 << height));

	last = prio_entry(prio_top(&root), struct num_node, node)->num;
	i = 0;
	gettimeofday(&start, NULL);
	while ((node = prio_top(&root))) {
		nn = prio_entry(node, struct num_node, node);
		if (last < nn->num)
			printf("fault\n");
		last = nn->num;
		prio_erase(node, &root, num_cmp);
		free(nn);
		++i;
	}
	gettimeofday(&end, NULL);
	if (i != n - 1)
		printf("fault\n");
	timersub(&end, &start, &delta);
	printf("time: %ld.%06ld\n", delta.tv_sec, delta.tv_usec);

	return 0;
}
