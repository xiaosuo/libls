AM_CFLAGS = -I$(top_srcdir)/include -Wall -Werror
LDADD = ../../src/.libs/libls.a

noinst_PROGRAMS = \
		  avltree \
		  rbtree \
		  heap \
		  prio_tree

avltree_SOURCES = avltree.c

rbtree_SOURCES = rbtree.c

heap_SOURCES = heap.c

prio_tree_SOURCES = prio_tree.c

EXTRA_DIST = benchmark.sh
