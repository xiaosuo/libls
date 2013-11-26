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
		  csv

list_SOURCES = list.c

rbtree_SOURCES = rbtree.c

bitmap_SOURCES = bitmap.c

pr_SOURCES = pr.c

avltree_SOURCES = avltree.c

file_SOURCES = file.c

csv_SOURCES = csv.c

EXTRA_DIST = \
	     sample.ini \
	     sample.csv
