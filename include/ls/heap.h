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
	unsigned int idx;

	heap->node[node->idx] = node2;
	heap->node[node2->idx] = node;
	idx = node->idx;
	node->idx = node2->idx;
	node2->idx = idx;
}

static inline void heap_pull_up(struct heap *heap, struct heap_node *node,
		heap_cmp_func_t *cmp)
{
	struct heap_node *parent;

	while (node->idx > 0) {
		parent = heap->node[(node->idx - 1) / 2];
		if (cmp(node, parent) >= 0)
			break;
		heap_swap(heap, node, parent);
	}
}

static inline void heap_push_down(struct heap *heap, struct heap_node *node,
		heap_cmp_func_t *cmp)
{
	struct heap_node *child, *min;
	unsigned int idx;

	while (1) {
		idx = node->idx * 2 + 1;
		if (idx >= heap->len)
			return;
		child = heap->node[idx];
		if (cmp(child, node) < 0)
			min = child;
		else
			min = node;
		if (++idx < heap->len) {
			child = heap->node[idx];
			if (cmp(child, min) < 0)
				min = child;
		}
		if (min == node)
			break;
		heap_swap(heap, node, min);
	}
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
	struct heap_node *last = heap->node[--heap->len];

	if (node != last) {
		heap->node[node->idx] = last;
		last->idx = node->idx;
		if (last->idx > 0 &&
		    cmp(last, heap->node[(last->idx - 1) / 2]) < 0)
			heap_pull_up(heap, last, cmp);
		else
			heap_push_down(heap, last, cmp);
	}
}

#endif /* __LS_HEAP_H */
