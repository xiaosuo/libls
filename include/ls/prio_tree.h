#ifndef __PRIO_TREE_H
#define __PRIO_TREE_H

#include <ls/stddef.h>

struct prio_node {
	unsigned long		__parent_balance;
	struct prio_node	*left, *right;
} __attribute__((aligned(sizeof(long))));

struct prio_root {
	struct prio_node	*node;
};

#define prio_entry(ptr, type, member) container_of(ptr, type, member)

typedef int prio_cmp_func_t(const struct prio_node *node,
		const struct prio_node *node2);

static inline struct prio_node *prio_top(struct prio_root *root)
{
	return root->node;
}

void prio_insert(struct prio_node *node, struct prio_root *root,
		prio_cmp_func_t *cmp);
void prio_adjust(struct prio_node *node, struct prio_root *root,
		prio_cmp_func_t *cmp);
void prio_erase(struct prio_node *node, struct prio_root *root,
		prio_cmp_func_t *cmp);

#endif /* __PRIO_TREE_H */
