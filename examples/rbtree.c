#include <ls.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>

struct num_node {
	struct rb_node	node;
	int		num;
};

struct num_node *num_search(struct rb_root *root, int num)
{
	struct rb_node *node = root->rb_node;

	while (node) {
		struct num_node *nn= container_of(node, struct num_node, node);

		if (num < nn->num)
			node = node->rb_left;
		else if (num > nn->num)
			node = node->rb_right;
		else
			return nn;
	}

	return NULL;
}

bool num_insert(struct rb_root *root, struct num_node *nn)
{
	struct rb_node **new = &(root->rb_node), *parent = NULL;

	while (*new) {
		struct num_node *this = container_of(*new, struct num_node,
				node);

		parent = *new;
		if (nn->num < this->num)
			new = &((*new)->rb_left);
		else if (nn->num > this->num)
			new = &((*new)->rb_right);
		else
			return false;
	}

	rb_link_node(&nn->node, parent, new);
	rb_insert_color(&nn->node, root);

	return true;
}

int main(int argc, char *argv[])
{
	struct rb_root root = RB_ROOT;
	struct num_node *nn;
	struct rb_node *node;
	int i;

	srand(time(NULL));
	for (i = 0; i < 10; i++) {
		nn = malloc(sizeof(*nn));
		if (!nn)
			break;
		do {
			nn->num = rand();
		} while (!num_insert(&root, nn));
	}

	for (node = rb_first(&root); node; node = rb_next(node))
		printf("%d\n", rb_entry(node, struct num_node, node)->num);

	while ((node = root.rb_node)) {
		rb_erase(node, &root);
		free(rb_entry(node, struct num_node, node));
	}

	return EXIT_SUCCESS;
}
