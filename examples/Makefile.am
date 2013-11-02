AM_CFLAGS = -I$(top_srcdir)/include -Wall -Werror
LDADD = ../src/.libs/libls.a

noinst_PROGRAMS = \
		  list \
		  rbtree \
		  bitmap \
		  ini \
		  pr \
		  avltree

list_SOURCES = list.c

rbtree_SOURCES = rbtree.c

bitmap_SOURCES = bitmap.c

pr_SOURCES = pr.c

avltree_SOURCES = avltree.c

EXTRA_DIST = sample.ini
