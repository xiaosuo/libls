#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libls], [1.0.0], [xiaosuo@gmail.com])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/rbtree.c])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_HEADERS([config.h])

AC_USE_SYSTEM_EXTENSIONS
AC_SYS_LARGEFILE

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_CC_C_O
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

LT_INIT

# Checks for libraries.
PKG_CHECK_MODULES([LIBSYSTEMD_DAEMON], [libsystemd-daemon])
AC_SUBST(LIBSYSTEMD_DAEMON_CFLAGS)
AC_SUBST(LIBSYSTEMD_DAEMON_LIBS)

# Checks for header files.
AC_CHECK_HEADERS([limits.h stddef.h stdint.h stdlib.h string.h strings.h sys/param.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([isascii memset strchr])

AC_CONFIG_FILES([Makefile
		 libls.pc
                 include/Makefile
		 examples/Makefile
                 src/Makefile])
AC_OUTPUT
AC_MSG_RESULT([
	$PACKAGE_NAME $VERSION
	PACKAGE_TARNAME:		${PACKAGE_TARNAME}
	prefix:				${prefix}
	exec_prefix:			${exec_prefix}
	bindir:				${bindir}
	sysconfdir:			${sysconfdir}
	libdir:				${libdir}
	datarootdir:			${datarootdir}
	docdir:				${docdir}
	includedir:			${includedir}

	CC:				${CC}
	CFLAGS:				${CFLAGS}
	CPPFLAGS:			${CPPFLAGS}
	LDFLAGS:			${LDFLAGS}
	LIBSYSTEMD_DAEMON_CFLAGS:	${LIBSYSTEMD_DAEMON_CFLAGS}
	LIBSYSTEMD_DAEMON_LIBS:		${LIBSYSTEMD_DAEMON_LIBS}
])
