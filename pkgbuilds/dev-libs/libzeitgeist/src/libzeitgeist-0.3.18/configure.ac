# When releasing also remember to update the soname as instructed below
AC_INIT(libzeitgeist, 0.3.18)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

CFLAGS="$CFLAGS"

# Before making a release, the ZEITGEIST_LT_VERSION string should be updated.
# The string is of the form C:R:A.
# - If interfaces have been changed or added, but binary compatibility has
#   been preserved, change to C+1:0:A+1
# - If binary compatibility has been broken (eg removed or changed interfaces)
#   change to C+1:0:0
# - If the interface is the same as the previous version, change to C:R+1:A
ZEITGEIST_LT_CURRENT=2
ZEITGEIST_LT_REV=4
ZEITGEIST_LT_AGE=1
ZEITGEIST_LT_VERSION="$ZEITGEIST_LT_CURRENT:$ZEITGEIST_LT_REV:$ZEITGEIST_LT_AGE"
ZEITGEIST_LT_LDFLAGS="-version-info $ZEITGEIST_LT_VERSION -export-symbols-regex '^zeitgeist_.*'"

AC_SUBST(ZEITGEIST_LT_VERSION)
AC_SUBST(ZEITGEIST_LT_LDFLAGS)

###################################################################
# Check essential build programs
###################################################################
AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AM_PROG_LIBTOOL


####################################################################
# Compiler generate debug code
####################################################################
AC_ARG_ENABLE([debug],
              AC_HELP_STRING([--enable-debug], [build with debug symbols]),,
              [enable_debug=no])

if test "x$enable_debug" = "xyes"; then
	CFLAGS="-g $CFLAGS"
	AC_DEFINE(ENABLE_DEBUG, 1, [build with extra debug information])
fi

AM_CONDITIONAL(ENABLE_DEBUG, test "$enable_debug" = "yes")



####################################################################
# Maintainer C flags
####################################################################
m4_define([maintainer_flags_default], [no])
AC_ARG_ENABLE([maintainer-flags],
              [AS_HELP_STRING([--enable-maintainer-flags=@<:@no/yes@:>@],[Use strict compiler flags @<:@default=no@:>@])],
              [],
              [enable_maintainer_flags=maintainer_flags_default])
AS_IF([test "x$enable_maintainer_flags" = "xyes" && test "x$GCC" = "xyes"],
      [
        MAINTAINER_CFLAGS="-g -Werror -Wcast-align -Wno-uninitialized -Wempty-body -Wformat-security -Winit-self"
      ]
)
AC_SUBST(MAINTAINER_CFLAGS)

####################################################################
# Check library deps
####################################################################
GLIB_REQUIRED=2.26
PKG_CHECK_MODULES(GLIB2, [glib-2.0 >= $GLIB_REQUIRED ])
PKG_CHECK_MODULES(GOBJECT2, [gobject-2.0 >= $GLIB_REQUIRED ])
PKG_CHECK_MODULES(GIO2, [gio-2.0 >= $GLIB_REQUIRED ])
PKG_CHECK_MODULES(GIO_UNIX2, [gio-unix-2.0])

ZEITGEIST_CFLAGS="$GLIB2_CFLAGS $GOBJECT2_CFLAGS $GIO2_CFLAGS $GIO_UNIX2_CFLAGS"
ZEITGEIST_LIBS="$GLIB2_LIBS $GOBJECT2_LIBS $GIO2_LIBS $GIO_UNIX2_LIBS"
AC_SUBST(ZEITGEIST_CFLAGS)
AC_SUBST(ZEITGEIST_LIBS)

GTK_DOC_CHECK(1.0)

AC_PATH_PROG(GLIB_MKENUMS, glib-mkenums)
AC_SUBST(GLIB_MKENUMS)

AC_PATH_PROG(GLIB_GENMARSHAL, glib-genmarshal)
AC_SUBST(GLIB_GENMARSHAL)

AC_OUTPUT([
zeitgeist-1.0.pc
Makefile
bindings/Makefile
doc/Makefile
doc/reference/Makefile
examples/Makefile
src/Makefile
tests/Makefile
])

AC_MSG_NOTICE([

  LibZeitgeist $VERSION
  ------------------

  Prefix           : ${prefix}

  Build API docs   : ${enable_gtk_doc}

  Extra CFlags     : ${CPPFLAGS} $MAINTAINER_CFLAGS
])

