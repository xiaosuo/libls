AM_CFLAGS = -I$(top_srcdir)/include -Wall -Werror $(LIBSYSTEMD_DAEMON_CFLAGS)
AM_LDFLAGS = $(LIBSYSTEMD_DAEMON_LIBS)
lib_LTLIBRARIES = libls.la
libls_la_SOURCES = \
		   rbtree.c \
		   bitmap.c \
		   hweight.c \
		   find_next_bit.c \
		   find_last_bit.c \
		   string.c \
		   hexdump.c \
		   ini.c \
		   avltree.c \
		   file.c \
		   csv.c \
		   daemon.c \
		   pr.c \
		   prio_tree.c \
		   compiler.c \
		   base64.c \
		   url_encode.c \
		   percent_encode.c \
		   loop.c
libls_la_LDFLAGS = -version-info 0:0:0
