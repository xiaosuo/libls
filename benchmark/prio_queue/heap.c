#include <ls.h>

#include <stdlib.h>
#include <time.h>

#include <sys/time.h>

struct num_node {
	int			num;
	struct heap_node	node;
};

static inline int num_cmp(const struct heap_node *node,
		const struct heap_node *node2)
{
	struct num_node *nn, *nn2;

	nn = heap_entry(node, struct num_node, node);
	nn2 = heap_entry(node2, struct num_node, node);

	if (nn->num < nn2->num)
		return -1;
	else if (nn->num > nn2->num)
		return 1;
	else
		return 0;
}

int main(int argc, char *argv[])
{
	struct heap heap;
	int i, last, n = atoi(argv[1]);
	struct num_node *nn;
	struct timeval start, end, delta;
	struct heap_node *node;

	heap_init(&heap);
	srand(time(NULL));
	gettimeofday(&start, NULL);
	for (i = 1; i < n; i++) {
		nn = malloc(sizeof(*nn));
		if (!nn)
			break;
		nn->num = rand();
		heap_insert(&heap, &nn->node, num_cmp);
	}
	gettimeofday(&end, NULL);
	printf("num: %d\n", i);
	timersub(&end, &start, &delta);
	printf("time: %ld.%06ld\n", delta.tv_sec, delta.tv_usec);

	last = heap_entry(heap_top(&heap), struct num_node, node)->num;
	i = 0;
	gettimeofday(&start, NULL);
	while ((node = heap_top(&heap))) {
		nn = heap_entry(node, struct num_node, node);
		if (last < nn->num)
			printf("fault\n");
		last = nn->num;
		heap_delete(&heap, node, num_cmp);
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
