AM_CFLAGS = -I$(top_srcdir)/include -Wall -Werror
LDADD = ../src/.libs/libls.a

noinst_PROGRAMS = \
		  list \
		  rbtree \
		  bitmap \
		  ini

list_SOURCES = list.c

rbtree_SOURCES = rbtree.c

bitmap_SOURCES = bitmap.c

EXTRA_DIST = sample.ini
