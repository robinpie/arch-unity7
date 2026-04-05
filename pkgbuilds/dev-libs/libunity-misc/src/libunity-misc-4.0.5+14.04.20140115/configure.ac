AC_PREREQ(2.59)

AC_INIT([libunity-misc],
        [4.0.4],
        [https://bugs.launchpad.net/avani])
AC_CONFIG_SRCDIR([unity-misc/na-tray.h])

AM_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE([1.9])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Before making a release, the UNITY_MISC_LT_VERSION string should be updated.
# The string is of the form C:R:A.
# - If interfaces have been changed or added, but binary compatibility has
#   been preserved, change to C+1:0:A+1
# - If binary compatibility has been broken (eg removed or changed interfaces)
#   change to C+1:0:0
# - If the interface is the same as the previous version, change to C:R+1:A
UNITY_MISC_LT_CURRENT=5
UNITY_MISC_LT_REV=0
UNITY_MISC_LT_AGE=1
UNITY_MISC_LT_VERSION="$UNITY_MISC_LT_CURRENT:$UNITY_MISC_LT_REV:$UNITY_MISC_LT_AGE"
UNITY_MISC_LT_LDFLAGS="-version-info $UNITY_MISC_LT_VERSION"

AC_SUBST(UNITY_MISC_LT_VERSION)
AC_SUBST(UNITY_MISC_LT_LDFLAGS)

dnl ===========================================================================

# Checks for programs
AC_PROG_CC
AM_PROG_CC_C_O
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

# Checks for header files
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures and compiler charecteristics
AC_C_CONST

# Checks for library functions
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([memset munmap strcasecmp strdup])

PKG_CHECK_MODULES(UNITY_MISC,
                  glib-2.0     >= 2.22
                  gobject-2.0  >= 2.22
                  gdk-x11-3.0 >= 3.0
                  gtk+-3.0 >= 3.0
                  gtk+-x11-3.0 >= 3.0
                  x11
                 )
AC_SUBST(UNITY_MISC_CFLAGS)
AC_SUBST(UNITY_MISC_LIBS)

dnl ===========================================================================

if test "x$GCC" = "xyes"; then
  GCC_FLAGS="-g -Wall"
fi
AC_SUBST(GCC_FLAGS)

MAINTAINER_CFLAGS="-Wno-error=deprecated-declarations -Wall -Wshadow -Wcast-align -Wno-uninitialized -Wempty-body -Wformat-security -Winit-self"
AC_SUBST(MAINTAINER_CFLAGS)

dnl = GTK Doc Check ===========================================================

GTK_DOC_CHECK([1.8])

dnl ===========================================================================
AC_CONFIG_FILES([
  Makefile
  unity-misc/Makefile
  doc/Makefile
  doc/reference/Makefile
  doc/reference/libunity-misc/Makefile
  examples/Makefile
  unity-misc.pc
])

AC_OUTPUT

echo ""
echo " Libunity-misc $VERSION"
echo " ==============================="
echo ""
echo " Prefix                   : ${prefix}"
echo ""
echo " Documentation            : ${enable_gtk_doc}"
echo ""
