AM_CFLAGS = -I$(top_srcdir)/include -Wall -Werror
LDADD = ../src/.libs/libls.a

noinst_PROGRAMS = \
		  list \
		  rbtree \
		  bitmap \
		  ini \
		  pr \
		  avltree \
		  file \
		  csv \
		  daemon \
		  heap \
		  base64 \
		  loop

list_SOURCES = list.c

rbtree_SOURCES = rbtree.c

bitmap_SOURCES = bitmap.c

pr_SOURCES = pr.c

avltree_SOURCES = avltree.c

file_SOURCES = file.c

csv_SOURCES = csv.c

daemon_SOURCES = daemon.c

heap_SOURCES = heap.c

base64_SOURCES = base64.c

loop_SOURCES = loop.c

EXTRA_DIST = \
	     sample.ini \
	     sample.csv
