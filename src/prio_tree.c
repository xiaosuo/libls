#include <stdlib.h>

#include <ls/prio_tree.h>

enum prio_balance {
	PRIO_EQUAL_WEIGHT,
	PRIO_LEFT_HEAVIER,
};

static inline struct prio_node *prio_parent(struct prio_node *node)
{
	return (struct prio_node *)(node->__parent_balance & ~1ul);
}

static inline enum prio_balance prio_balance(struct prio_node *node)
{
	return node->__parent_balance & 1ul;
}

static inline void prio_set_parent(struct prio_node *node,
		struct prio_node *parent)
{
	node->__parent_balance = prio_balance(node) | (unsigned long)parent;
}

static inline void prio_set_balance(struct prio_node *node,
		enum prio_balance balance)
{
	node->__parent_balance = (unsigned long)prio_parent(node) | balance;
}

static inline void prio_init_parent(struct prio_node *node,
		struct prio_node *parent)
{
	node->__parent_balance = (unsigned long)parent;
}

static inline void prio_change_balance(struct prio_node *node)
{
	node->__parent_balance ^= 1ul;
}

static inline void prio_update_downlink(struct prio_node *node,
		const struct prio_node *old, struct prio_root *root)
{
	struct prio_node *parent = prio_parent(node);

	if (parent) {
		if (parent->left == old)
			parent->left = node;
		else
			parent->right = node;
	} else {
		root->node = node;
	}
}

static void prio_swap(struct prio_node *node,
		struct prio_node *parent, struct prio_root *root)
{
	enum prio_balance balance;

	if (parent->left == node) {
		swap(node->right, parent->right);
		if (node->right)
			prio_set_parent(node->right, node);
		if (parent->right)
			prio_set_parent(parent->right, parent);
		parent->left = node->left;
		if (parent->left)
			prio_set_parent(parent->left, parent);
		prio_set_parent(node, prio_parent(parent));
		prio_update_downlink(node, parent, root);
		node->left = parent;
		prio_set_parent(parent, node);
	} else {
		swap(node->left, parent->left);
		if (node->left)
			prio_set_parent(node->left, node);
		if (parent->left)
			prio_set_parent(parent->left, parent);
		parent->right = node->right;
		if (parent->right)
			prio_set_parent(parent->right, parent);
		prio_set_parent(node, prio_parent(parent));
		prio_update_downlink(node, parent, root);
		node->right = parent;
		prio_set_parent(parent, node);
	}

	balance = prio_balance(node);
	prio_set_balance(node, prio_balance(parent));
	prio_set_balance(parent, balance);
}

void prio_insert(struct prio_node *node, struct prio_root *root,
		prio_cmp_func_t *cmp)
{
	struct prio_node *parent;

	node->left = node->right = NULL;
	if (!root->node) {
		node->__parent_balance = 0;
		root->node = node;
		return;
	}

	parent = root->node;
	while (1) {
		switch (prio_balance(parent)) {
		case PRIO_EQUAL_WEIGHT:
			if (!parent->left) {
				parent->left = node;
				break;
			}
			parent = parent->left;
			continue;
		case PRIO_LEFT_HEAVIER:
			if (!parent->right) {
				parent->right = node;
				break;
			}
			parent = parent->right;
			continue;
		default:
			abort();
		}
		break;
	}
	prio_init_parent(node, parent);

	do {
		if (cmp(node, parent) <= 0)
			break;
		prio_change_balance(parent);
		prio_swap(node, parent, root);
		parent = prio_parent(node);
	} while (parent);

	while (parent) {
		prio_change_balance(parent);
		parent = prio_parent(parent);
	}
}

void prio_adjust(struct prio_node *node, struct prio_root *root,
		prio_cmp_func_t *cmp)
{
	struct prio_node *parent;

	parent = prio_parent(node);
	if (parent && cmp(node, parent) > 0) {
		do {
			prio_swap(node, parent, root);
			parent = prio_parent(node);
		} while (parent && cmp(node, parent) > 0);
	} else {
		struct prio_node *max;

		while (1) {
			max = node;
			if (node->left && cmp(node->left, max) > 0)
				max = node->left;
			if (node->right && cmp(node->right, max) > 0)
				max = node->right;
			if (max == node)
				break;
			prio_swap(max, node, root);
		}
	}
}

void prio_erase(struct prio_node *node, struct prio_root *root,
		prio_cmp_func_t *cmp)
{
	struct prio_node *victim, *parent = NULL, *child;

	while (1) {
		child = node->left;
		if (!child)
			break;
		if (node->right && cmp(node->right, child) > 0)
			child = node->right;
		prio_swap(child, node, root);
	}

	victim = root->node;
	while (1) {
		switch (prio_balance(victim)) {
		case PRIO_EQUAL_WEIGHT:
			if (!victim->right)
				break;
			parent = victim;
			victim = parent->right;
			continue;
		case PRIO_LEFT_HEAVIER:
			parent = victim;
			victim = parent->left;
			continue;
		default:
			abort();
		}
		break;
	}

	if (parent) {
		if (parent->left == victim)
			parent->left = NULL;
		else
			parent->right = NULL;
	} else {
		root->node = NULL;
	}

	while (parent) {
		prio_change_balance(parent);
		parent = prio_parent(parent);
	}

	if (victim != node) {
		parent = prio_parent(node);
		if (parent) {
			if (parent->left == node)
				parent->left = victim;
			else
				parent->right = victim;
		} else {
			root->node = victim;
		}
		victim->__parent_balance = node->__parent_balance;
		for (parent = prio_parent(victim); parent;
				parent = prio_parent(victim)) {
			if (cmp(victim, parent) <= 0)
				break;
			prio_swap(victim, parent, root);
		}
	}
}
