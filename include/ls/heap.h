#ifndef __LS_HEAP_H
#define __LS_HEAP_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ls/stddef.h>

#define heap_entry(ptr, type, member) container_of(ptr, type, member)

struct heap_node {
	unsigned int	idx;
};

struct heap {
	struct heap_node	**node;
	unsigned int		len, size;
};

typedef int heap_cmp_func_t(const struct heap_node *node,
		const struct heap_node *node2);

static inline void heap_init(struct heap *heap)
{
	heap->node = NULL;
	heap->len = heap->size = 0;
}

static inline void heap_destroy(struct heap *heap)
{
	assert(heap->len == 0);
	free(heap->node);
}

static inline void heap_swap(struct heap *heap, struct heap_node *node,
		struct heap_node *node2)
{
	heap->node[node->idx] = node2;
	heap->node[node2->idx] = node;
	swap(node->idx, node2->idx);
}

static inline void heap_pull_up(struct heap *heap, struct heap_node *node,
		heap_cmp_func_t *cmp)
{
	struct heap_node *parent;

	while (node->idx > 0) {
		parent = heap->node[(node->idx - 1) / 2];
		if (cmp(node, parent) <= 0)
			break;
		heap_swap(heap, node, parent);
	}
}

static inline void heap_push_down(struct heap *heap, struct heap_node *node,
		heap_cmp_func_t *cmp)
{
	struct heap_node *child, *max;
	unsigned int idx;

	while (1) {
		idx = node->idx * 2 + 1;
		if (idx >= heap->len)
			return;
		child = heap->node[idx];
		if (cmp(child, node) > 0)
			max = child;
		else
			max = node;
		if (++idx < heap->len) {
			child = heap->node[idx];
			if (cmp(child, max) > 0)
				max = child;
		}
		if (max == node)
			break;
		heap_swap(heap, node, max);
	}
}

static inline void heap_adjust(struct heap *heap, struct heap_node *node,
		heap_cmp_func_t *cmp)
{
	if (node->idx > 0 && cmp(node, heap->node[(node->idx - 1) / 2]) >= 0)
		heap_pull_up(heap, node, cmp);
	else
		heap_push_down(heap, node, cmp);
}

static inline int heap_insert(struct heap *heap, struct heap_node *node,
		heap_cmp_func_t *cmp)
{
	if (heap->len >= heap->size) {
		struct heap_node **new_node;
		unsigned int new_size = heap->size * 2;

		if (!new_size)
			new_size = 1;
		new_node = realloc(heap->node, sizeof(*new_node) * new_size);
		if (!new_node)
			return -1;
		heap->node = new_node;
		heap->size = new_size;
	}
	node->idx = heap->len;
	heap->node[heap->len++] = node;
	heap_pull_up(heap, node, cmp);

	return 0;
}

static inline struct heap_node *heap_top(struct heap *heap)
{
	return heap->len > 0 ? heap->node[0] : NULL;
}

static inline void heap_delete(struct heap *heap, struct heap_node *node,
		heap_cmp_func_t *cmp)
{
	unsigned int p = node->idx, c;
	struct heap_node *last;

	if (--heap->len == 0)
		return;
	last = heap->node[heap->len];
	if (last == node)
		return;
	while (1) {
		c = p * 2 + 1;
		if (c > heap->len)
			break;
		node = heap->node[c];
		if (++c <= heap->len && cmp(heap->node[c], node) > 0)
			node = heap->node[c];
		heap->node[p] = node;
		swap(node->idx, p);
	}
	if (last != node) {
		heap->node[p] = last;
		last->idx = p;
		heap_pull_up(heap, last, cmp);
	}
}

#endif /* __LS_HEAP_H */
