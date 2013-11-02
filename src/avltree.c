#include <ls/stddef.h>
#include <ls/avltree.h>

#include <assert.h>
#include <stdlib.h>

/* Accessors */

static inline void avl_set_parent(struct avl_node *node,
		struct avl_node *parent)
{
	node->__parent_balance = ((unsigned long)parent) |
		(node->__parent_balance & 3);
}

static inline void avl_set_balance(struct avl_node *node, int balance)
{
	assert(balance >= 0);
	assert(balance <= 2);

	node->__parent_balance = (node->__parent_balance & ~3) | balance;
}

static inline struct avl_node *avl_parent(const struct avl_node *node)
{
	return (struct avl_node *)(node->__parent_balance & ~3);
}

static inline int avl_balance(const struct avl_node *node)
{
	return node->__parent_balance & 3;
}

/* Iterations */

static inline struct avl_node *avl_left_most(const struct avl_node *node)
{
	while (node->left)
		node = node->left;

	return (struct avl_node *)node;
}

static inline struct avl_node *avl_right_most(const struct avl_node *node)
{
	while (node->right)
		node = node->right;

	return (struct avl_node *)node;
}

struct avl_node *avl_next(const struct avl_node *node)
{
	struct avl_node *parent;

	assert(node != NULL);
	assert(!AVL_EMTPY_NODE(node));

	if (node->right)
		return avl_left_most(node->right);

	while ((parent = avl_parent(node)) && node == parent->right)
		node = parent;

	return parent;
}

struct avl_node *avl_prev(const struct avl_node *node)
{
	struct avl_node *parent;

	assert(node != NULL);
	assert(!AVL_EMTPY_NODE(node));

	if (node->left)
		return avl_right_most(node->left);

	while ((parent = avl_parent(node)) && node == parent->left)
		node = parent;

	return parent;
}

struct avl_node *avl_first(const struct avl_root *root)
{
	struct avl_node *node = root->node;

	if (node)
		node = avl_left_most(node);

	return node;
}

struct avl_node *avl_last(const struct avl_root *root)
{
	struct avl_node *node = root->node;

	if (node)
		node = avl_right_most(node);

	return node;
}

static inline struct avl_node *avl_left_deepest(const struct avl_node *node)
{
	for (;;) {
		if (node->left)
			node = node->left;
		else if (node->right)
			node = node->right;
		else
			return (struct avl_node *)node;
	}
}

struct avl_node *avl_first_postorder(const struct avl_root *root)
{
	struct avl_node *node = root->node;

	if (node)
		node = avl_left_deepest(node);

	return node;
}

struct avl_node *avl_next_postorder(const struct avl_node *node)
{
	struct avl_node *parent;

	if (!node)
		return NULL;
	parent = avl_parent(node);
	if (parent && node == parent->left && parent->right)
		return avl_left_deepest(parent->right);
	else
		return parent;
}

/* Helper functions */

static inline void avl_change_child(const struct avl_node *node,
		const struct avl_node *child,
		struct avl_node *parent, struct avl_root *root)
{
	if (!parent)
		root->node = (struct avl_node *)node;
	else if (parent->left == child)
		parent->left = (struct avl_node *)node;
	else
		parent->right = (struct avl_node *)node;
}

/**
 *      P                  N
 *     / \                / \
 *    L   N      -->     P  NR
 *       / \            / \
 *      NL NR          L  NL
 */
static void avl_rotate_left(struct avl_node *node, struct avl_node *parent,
		struct avl_root *root)
{
	assert(node != NULL);
	assert(parent != NULL);

	parent->right = node->left;
	if (parent->right)
		avl_set_parent(parent->right, parent);
	node->left = parent;
	avl_set_parent(node, avl_parent(parent));
	avl_set_parent(parent, node);
	avl_change_child(node, parent, avl_parent(node), root);
}

/**
 *      P                  N
 *     / \                / \
 *    N   R      -->     NL  P
 *   / \                    / \
 *  NL NR                  NR  R
 */
static void avl_rotate_right(struct avl_node *node, struct avl_node *parent,
		struct avl_root *root)
{
	assert(node != NULL);
	assert(parent != NULL);

	parent->left = node->right;
	if (parent->left)
		avl_set_parent(parent->left, parent);
	node->right = parent;
	avl_set_parent(node, avl_parent(parent));
	avl_set_parent(parent, node);
	avl_change_child(node, parent, avl_parent(node), root);
}

static void avl_left_balance(struct avl_node *node, struct avl_node *parent,
		struct avl_root *root)
{
	switch (avl_balance(node)) {
	case AVL_EQUAL_HEIGHT:
		abort();
	case AVL_LEFT_HIGHER:
		/* Left left case */
		avl_set_balance(node, AVL_EQUAL_HEIGHT);
		avl_set_balance(parent, AVL_EQUAL_HEIGHT);
		break;
	case AVL_RIGHT_HIGHER:
		/**
		 * Left right case:
		 *
		 *        P                    NR
		 *       / \                 /   \
		 *      N   R               N     P
		 *     / \         -->     / \   / \
		 *    NL NR              NL NRL NRR R
		 *       / \
		 *     NRL NRR
		 */
		switch (avl_balance(node->right)) {
		case AVL_EQUAL_HEIGHT:
			avl_set_balance(node, AVL_EQUAL_HEIGHT);
			avl_set_balance(parent, AVL_EQUAL_HEIGHT);
			break;
		case AVL_LEFT_HIGHER:
			avl_set_balance(node, AVL_EQUAL_HEIGHT);
			avl_set_balance(parent, AVL_RIGHT_HIGHER);
			break;
		case AVL_RIGHT_HIGHER:
			avl_set_balance(node, AVL_LEFT_HIGHER);
			avl_set_balance(parent, AVL_EQUAL_HEIGHT);
			break;
		default:
			abort();
		}
		avl_set_balance(node->right, AVL_EQUAL_HEIGHT);
		/* Left right case to left left case */
		avl_rotate_left(node->right, node, root);
		node = parent->left;
		break;
	default:
		abort();
	}
	avl_rotate_right(node, parent, root);
}

static void avl_right_balance(struct avl_node *node, struct avl_node *parent,
		struct avl_root *root)
{
	switch (avl_balance(node)) {
	case AVL_EQUAL_HEIGHT:
		abort();
	case AVL_LEFT_HIGHER:
		/**
		 * Right left case:
		 *
		 *    P                    NL
		 *   / \                 /   \
		 *  L   N               P     N
		 *     / \     -->     / \   / \
		 *    NL NR           L NLL NLR NR
		 *   / \
		 * NLL NLR
		 */
		switch (avl_balance(node->left)) {
		case AVL_EQUAL_HEIGHT:
			avl_set_balance(node, AVL_EQUAL_HEIGHT);
			avl_set_balance(parent, AVL_EQUAL_HEIGHT);
			break;
		case AVL_LEFT_HIGHER:
			avl_set_balance(node, AVL_RIGHT_HIGHER);
			avl_set_balance(parent, AVL_EQUAL_HEIGHT);
			break;
		case AVL_RIGHT_HIGHER:
			avl_set_balance(node, AVL_EQUAL_HEIGHT);
			avl_set_balance(parent, AVL_LEFT_HIGHER);
			break;
		default:
			abort();
		}
		avl_set_balance(node->left, AVL_EQUAL_HEIGHT);
		/* Right left case to right right case */
		avl_rotate_right(node->left, node, root);
		node = parent->right;
		break;
	case AVL_RIGHT_HIGHER:
		/* Right Right case */
		avl_set_balance(node, AVL_EQUAL_HEIGHT);
		avl_set_balance(parent, AVL_EQUAL_HEIGHT);
		break;
	default:
		abort();
	}
	avl_rotate_left(node, parent, root);
}

void avl_insert_balance(struct avl_node *node, struct avl_root *root)
{
	struct avl_node *parent;

	while ((parent = avl_parent(node))) {
		if (parent->left == node) {
			switch (avl_balance(parent)) {
			case AVL_EQUAL_HEIGHT:
				avl_set_balance(parent, AVL_LEFT_HIGHER);
				break;
			case AVL_LEFT_HIGHER:
				avl_left_balance(node, parent, root);
				return;
			case AVL_RIGHT_HIGHER:
				avl_set_balance(parent, AVL_EQUAL_HEIGHT);
				return;
			default:
				abort();
			}
		} else {
			switch (avl_balance(parent)) {
			case AVL_EQUAL_HEIGHT:
				avl_set_balance(parent, AVL_RIGHT_HIGHER);
				break;
			case AVL_LEFT_HIGHER:
				avl_set_balance(parent, AVL_EQUAL_HEIGHT);
				return;
			case AVL_RIGHT_HIGHER:
				avl_right_balance(node, parent, root);
				return;
			default:
				abort();
			}
		}
		node = parent;
	}
}

static void avl_erase_right_left(struct avl_node *node, struct avl_node *parent,
		struct avl_root *root)
{
	struct avl_node *left = node->left;

	switch (avl_balance(left)) {
	case AVL_EQUAL_HEIGHT:
		avl_set_balance(node, AVL_EQUAL_HEIGHT);
		avl_set_balance(parent, AVL_EQUAL_HEIGHT);
		break;
	case AVL_LEFT_HIGHER:
		avl_set_balance(node, AVL_RIGHT_HIGHER);
		avl_set_balance(parent, AVL_EQUAL_HEIGHT);
		break;
	case AVL_RIGHT_HIGHER:
		avl_set_balance(node, AVL_EQUAL_HEIGHT);
		avl_set_balance(parent, AVL_LEFT_HIGHER);
		break;
	default:
		abort();
	}
	avl_set_balance(left, AVL_EQUAL_HEIGHT);
	avl_rotate_right(left, node, root);
	avl_rotate_left(left, parent, root);
}

static void avl_erase_left_right(struct avl_node *node, struct avl_node *parent,
		struct avl_root *root)
{
	struct avl_node *right = node->right;

	switch (avl_balance(right)) {
	case AVL_EQUAL_HEIGHT:
		avl_set_balance(node, AVL_EQUAL_HEIGHT);
		avl_set_balance(parent, AVL_EQUAL_HEIGHT);
		break;
	case AVL_LEFT_HIGHER:
		avl_set_balance(node, AVL_EQUAL_HEIGHT);
		avl_set_balance(parent, AVL_RIGHT_HIGHER);
		break;
	case AVL_RIGHT_HIGHER:
		avl_set_balance(node, AVL_LEFT_HIGHER);
		avl_set_balance(parent, AVL_EQUAL_HEIGHT);
		break;
	default:
		abort();
	}
	avl_set_balance(right, AVL_EQUAL_HEIGHT);
	avl_rotate_left(right, node, root);
	avl_rotate_right(right, parent, root);
}

static void avl_erase_balance(struct avl_node *node, struct avl_root *root)
{
	struct avl_node *parent;

	while ((parent = avl_parent(node))) {
		if (parent->left == node) {
			switch (avl_balance(parent)) {
			case AVL_EQUAL_HEIGHT:
				avl_set_balance(parent, AVL_RIGHT_HIGHER);
				return;
			case AVL_LEFT_HIGHER:
				avl_set_balance(parent, AVL_EQUAL_HEIGHT);
				node = parent;
				break;
			case AVL_RIGHT_HIGHER:
				node = parent->right;
				switch (avl_balance(node)) {
				case AVL_EQUAL_HEIGHT:
					/* Case 3a */
					avl_set_balance(node, AVL_LEFT_HIGHER);
					avl_rotate_left(node, parent, root);
					return;
				case AVL_LEFT_HIGHER:
					/* Case 3c */
					avl_erase_right_left(node, parent,
							root);
					node = avl_parent(node);
					break;
				case AVL_RIGHT_HIGHER:
					/* Case 3b */
					avl_set_balance(node, AVL_EQUAL_HEIGHT);
					avl_set_balance(parent,
							AVL_EQUAL_HEIGHT);
					avl_rotate_left(node, parent, root);
					break;
				default:
					abort();
				}
				break;
			default:
				abort();
			}
		} else {
			switch (avl_balance(parent)) {
			case AVL_EQUAL_HEIGHT:
				avl_set_balance(parent, AVL_LEFT_HIGHER);
				return;
			case AVL_LEFT_HIGHER:
				node = parent->left;
				switch (avl_balance(node)) {
				case AVL_EQUAL_HEIGHT:
					/* Case 3a */
					avl_set_balance(node, AVL_RIGHT_HIGHER);
					avl_rotate_right(node, parent, root);
					return;
				case AVL_LEFT_HIGHER:
					/* Case 3b */
					avl_set_balance(node, AVL_EQUAL_HEIGHT);
					avl_set_balance(parent,
							AVL_EQUAL_HEIGHT);
					avl_rotate_right(node, parent, root);
					break;
				case AVL_RIGHT_HIGHER:
					/* Case 3c */
					avl_erase_left_right(node, parent,
							root);
					node = avl_parent(node);
					break;
				default:
					abort();
				}
				break;
			case AVL_RIGHT_HIGHER:
				avl_set_balance(parent, AVL_EQUAL_HEIGHT);
				node = parent;
				break;
			default:
				abort();
			}
		}
	}
}

void avl_erase(struct avl_node *node, struct avl_root *root)
{
	struct avl_node *parent, *real = node;

	/**
	 * Reduce the problem to the case when the node node to be removed has
	 * at most one child.
	 */
	if (node->left && node->right)
		real = avl_right_most(node->left);

	avl_erase_balance(real, root);

	/* Unlink the real node */
	parent = avl_parent(real);
	avl_change_child(real->left ? real->left : real->right, real, parent,
			root);
	if (real->left)
		avl_set_parent(real->left, parent);
	else if (real->right)
		avl_set_parent(real->right, parent);

	if (node != real)
		avl_replace_node(node, real, root);
}

void avl_replace_node(struct avl_node *victim, struct avl_node *node,
		struct avl_root *root)
{
	*node = *victim;
	avl_change_child(node, victim, avl_parent(node), root);
	if (node->left)
		avl_set_parent(node->left, node);
	if (node->right)
		avl_set_parent(node->right, node);
}
