#ifndef __LS_AVLTREE_H
#define __LS_AVLTREE_H

enum {
	AVL_EQUAL_HEIGHT,
	AVL_LEFT_HIGHER,
	AVL_RIGHT_HIGHER,
};

struct avl_node {
	unsigned long	__parent_balance;
	struct avl_node	*left, *right;
};

struct avl_root {
	struct avl_node	*node;
};

#define AVL_ROOT (struct avl_root) { NULL, }
#define avl_entry(ptr, type, member) container_of(ptr, type, member)
#define AVL_EMPTY_ROOT(root) ((root)->node == NULL)

#define AVL_EMTPY_NODE(node) \
	((node)->__parent_balance == (unsigned long)(node))
#define AVL_CLEAR_NODE(node) \
	((node)->__parent_balance = (unsigned long)(node))

static inline void avl_set_parent_balance(struct avl_node *node,
		struct avl_node *parent, int balance)
{
	node->__parent_balance = ((unsigned long)parent) | balance;
}

static inline void avl_link_node(struct avl_node *node, struct avl_node *parent,
		struct avl_node **pos)
{
	node->left = node->right = NULL;
	avl_set_parent_balance(node, parent, AVL_EQUAL_HEIGHT);
	*pos = node;
}

void avl_insert_balance(struct avl_node *node, struct avl_root *root);
void avl_erase(struct avl_node *node, struct avl_root *root);

struct avl_node *avl_next(const struct avl_node *node);
struct avl_node *avl_prev(const struct avl_node *node);
struct avl_node *avl_first(const struct avl_root *root);
struct avl_node *avl_last(const struct avl_root *root);

struct avl_node *avl_first_postorder(const struct avl_root *root);
struct avl_node *avl_next_postorder(const struct avl_node *node);

/**
 * avl_postorder_for_each_entry_safe - iterate over avl_root in post order of
 * given type safe against removal of avl_node entry
 *
 * @pos:	the 'type *' to use as a loop cursor.
 * @n:		another 'type *' to use as temporary storage
 * @root:	'avl_root *' of the avltree.
 * @field:	the name of the avl_node field within 'type'.
 */
#define avl_postorder_for_each_entry_safe(pos, n, root, field) \
	for (pos = avl_entry(avl_first_postorder(root), typeof(*pos), field),\
		n = avl_entry(avl_next_postorder(&pos->field), \
			typeof(*pos), field); \
	     &pos->field; \
	     pos = n, \
		n = avl_entry(avl_next_postorder(&pos->field), \
			typeof(*pos), field))

void avl_replace_node(struct avl_node *victim, struct avl_node *node,
		struct avl_root *root);

#endif /* __LS_AVLTREE_H */
