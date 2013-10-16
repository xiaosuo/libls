ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

SUBDIRS = \
	  src \
	  include \
	  examples

dist_doc_DATA = \
		README.md \
		LICENSE.txt

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libls.pc
DISTCLEANFILES = libls.pc
EXTRA_DIST = libls.pc.in
