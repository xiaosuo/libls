#include <ls.h>
#include <time.h>

struct num_node {
	struct heap_node	node;
	int			num;

};

static inline int num_cmp(const struct heap_node *node, const struct heap_node *node2)
{
	int n, n2;

	n = heap_entry(node, struct num_node, node)->num;
	n2 = heap_entry(node2, struct num_node, node)->num;
	if (n < n2)
		return -1;
	else if (n > n2)
		return 1;
	else
		return 0;
}

int main(int argc, char *argv[])
{
	struct num_node *nn;
	int i;
	struct heap h;
	struct heap_node *n;

	heap_init(&h);
	srand(time(NULL));
	for (i = 0; i < 10; i++) {
		nn = malloc(sizeof(*nn));
		if (!nn)
			break;
		nn->num = rand();
		if (heap_insert(&h, &nn->node, num_cmp)) {
			free(nn);
			break;
		}
	}
	while ((n = heap_top(&h))) {
		heap_delete(&h, n, num_cmp);
		nn = heap_entry(n, struct num_node, node);
		printf("%d\n", nn->num);
		free(nn);
	}
	heap_destroy(&h);

	return 0;
}
