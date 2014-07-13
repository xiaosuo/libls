#include <ls.h>
#include <assert.h>
#include <time.h>

struct num_node {
	struct avl_node	node;
	int		num;
};

static void __permute(int *head, int size, int *curr, int len,
		void (*callback)(int *head, int size))
{
	int i = 0;

	if (len < 0)
		return;

	if (len == 1) {
		callback(head, size);
		return;
	}

	for (i = 0; i < len; i++) {
		if (i != 0)
			swap(curr[0], curr[i]);
		__permute(head, size, curr + 1, len - 1, callback);
		if (i != 0)
			swap(curr[0], curr[i]);
	}
}

static void permute(int *head, int size, void (*callback)(int *head, int size))
{
	__permute(head, size, head, size, callback);
}

static bool num_insert(struct num_node *node, struct avl_root *root)
{
	struct avl_node **pos = &(root->node), *parent = NULL;

	while (*pos) {
		struct num_node *n = avl_entry(*pos, struct num_node, node);

		parent = *pos;
		if (node->num < n->num)
			pos = &((*pos)->left);
		else if (node->num > n->num)
			pos = &((*pos)->right);
		else
			return false;
	}
	avl_link_node(&node->node, parent, pos);
	avl_insert_balance(&node->node, root);

	return true;
}

static struct num_node *num_search(int num, struct avl_root *root)
{
	struct avl_node *node = root->node;
	
	while (node) {
		struct num_node *n = avl_entry(node, struct num_node, node);

		if (num < n->num)
			node = node->left;
		else if (num > n->num)
			node = node->right;
		else
			return n;
	}

	return NULL;
}

static int check_height(struct avl_node *node)
{
	int left_height = 0, right_height = 0;

	if (node->left)
		left_height = check_height(node->left);
	if (node->right)
		right_height = check_height(node->right);

	assert(abs(left_height - right_height) <= 1);

	return 1 + MAX(left_height, right_height);
}

int check_inorder(struct avl_root *root)
{
	struct avl_node *node;
	int n = 0, num __attribute__((unused)) = 0;
	struct num_node *nm;

	for (node = avl_first(root); node; node = avl_next(node)) {
		nm = avl_entry(node, struct num_node, node);
		if (n++ != 0)
			assert(num < nm->num);
		num = nm->num;
	}

	return n;
}

int check_revorder(struct avl_root *root)
{
	struct avl_node *node;
	int n = 0, num __attribute__((unused)) = 0;
	struct num_node *nm;

	for (node = avl_last(root); node; node = avl_prev(node)) {
		nm = avl_entry(node, struct num_node, node);
		if (n++ != 0)
			assert(num > nm->num);
		num = nm->num;
	}

	return n;
}

int check_postorder(struct avl_root *root)
{
	struct num_node *node, *tmp;
	int n = 0;

	avl_postorder_for_each_entry_safe(node, tmp, root, node) {
		free(node);
		n++;
	}

	return n;
}

static void test1(int *head, int size)
{
	int i;
	struct num_node *node;
	struct avl_root root = AVL_ROOT;

	for (i = 0; i < size; i++) {
		node = malloc(sizeof(*node));
		assert(node);
		node->num = head[i];
		num_insert(node, &root);
		printf("%d ", head[i]);
	}
	printf("\n");

	assert(root.node);
	assert(avl_entry(root.node, struct num_node, node)->num == 4);
	assert(root.node->left);
	assert(avl_entry(root.node->left, struct num_node, node)->num == 3);
	assert(root.node->right);
	assert(avl_entry(root.node->right, struct num_node, node)->num == 5);
	assert(check_postorder(&root) == 3);
}

static void test2(int *head, int size)
{
	int i;
	struct num_node *node;
	struct avl_root root = AVL_ROOT;
	
	for (i = 0; i < size; i++) {
		node = malloc(sizeof(*node));
		assert(node);
		node->num = head[i];
		num_insert(node, &root);
		printf("%d ", head[i]);
	}
	printf("\n");
	check_height(root.node);
	assert(check_inorder(&root) == 10);
	assert(check_revorder(&root) == 10);
	assert(check_postorder(&root) == 10);
}

int main(int argc, char *argv[])
{
	int arr[100];
	int i;
	struct avl_root root = AVL_ROOT;
	struct num_node *nm;

	for (i = 0; i < 10; i++)
		arr[i] = 3 + i;

	permute(arr, 3, test1);
	permute(arr, 10, test2);

	srand(time(NULL));
	for (i = 0; i < 100; i++) {
		nm = malloc(sizeof(*nm));
		assert(nm);
		do {
			nm->num = rand();
		} while (!num_insert(nm, &root));
		arr[i] = nm->num;
	}
	while (i > 0) {
		int idx;

		check_height(root.node);
		assert(check_inorder(&root) == i);
		assert(check_revorder(&root) == i);
		idx = ((unsigned int)rand()) % i;
		--i;
		if (idx != i)
			swap(arr[idx], arr[i]);
		nm = num_search(arr[i], &root);
		avl_erase(&nm->node, &root);
		free(nm);
	}
	assert(AVL_EMPTY_ROOT(&root));

	return EXIT_SUCCESS;
}
