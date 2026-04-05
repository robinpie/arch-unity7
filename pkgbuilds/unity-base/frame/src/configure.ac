# Initialize Autoconf
AC_PREREQ([2.60])
AC_INIT([Touch Frame Library],
        [2.5.0],
        [],
        [frame])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([config-aux])
AC_CONFIG_MACRO_DIR([m4])

# Initialize Automake
AM_INIT_AUTOMAKE([1.11 foreign dist-xz])
AM_SILENT_RULES([yes])
AM_MAINTAINER_MODE([enable])

LIB_VERSION=6:0:0
AC_SUBST([LIB_VERSION])

# Initialize libtool
AC_PROG_LIBTOOL

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL


# *** Enable/disable the X11 backend ***
AC_ARG_ENABLE(x11,
              AC_HELP_STRING(--disable-x11, Do not build the X11 backend. (default: enable if available)),
              [],
              [enable_x11=auto])


# Check for X11 as an optional dependency
AS_IF([test "x$enable_x11" = "xauto"],
      [
        PKG_CHECK_MODULES(XINPUT,
                          x11 xext xorg-server [xi >= 1.5.99.1] [inputproto >= 2.1.99.6],
                          [have_xinput=yes],
                          [have_xinput=no])
      ])

AS_IF([test "x$enable_x11" = "xyes"],
      [
        PKG_CHECK_MODULES(XINPUT,
                          x11 xext xorg-server [xi >= 1.5.99.1] [inputproto >= 2.1.99.6],
                          [have_xinput=yes],
                          [
                            AC_MSG_ERROR([x11 and/or XInput libraries not found!])
                            have_xinput=no
                          ])
      ])

AS_IF([test "x$enable_x11" = "xno"],
      [have_xinput=no])

AS_IF([test "x$have_xinput" = "xyes"],
      [
        FRAME_X11_BACKEND="FRAME_X11_BACKEND"
        AM_CONDITIONAL(HAVE_XINPUT, true)
      ],
      [
        FRAME_X11_BACKEND="FRAME_NO_X11_BACKEND"
        AM_CONDITIONAL(HAVE_XINPUT, false)
      ])
AC_SUBST(FRAME_X11_BACKEND)
AC_SUBST(HAVE_XINPUT)

AC_CHECK_PROG([ASCIIDOC], [a2x], [a2x])
AM_CONDITIONAL([HAVE_DOCTOOLS], [test "x$ASCIIDOC" != "x"])
AS_IF([test "x$ASCIIDOC" = "x"],
      [AC_MSG_WARN([asciidoc not installed, man pages will not be created])])


# Check for Google C++ Testing Framework
CHECK_GTEST()
AM_CONDITIONAL([HAVE_GTEST], [test "x$have_gtest" = xyes])

# Check for TDD tools
AC_ARG_ENABLE([integration-tests],
              [AS_HELP_STRING([--enable-integration-tests],
                  [Enable building integration test cases (default: auto)])],
              [enable_integration_tests=$enableval],
              [enable_integration_tests=auto])

AS_IF([test "x$enable_integration_tests" != xno],
      [CHECK_XORG_GTEST(
          [AC_MSG_NOTICE([xorg-gtest is available, integration tests will be built])],
          [AC_MSG_WARN([xorg-gtest is not available, tests will not be built])])],
      [have_xorg_gtest=no])

AM_CONDITIONAL([ENABLE_INTEGRATION_TESTS], [test "x$have_xorg_gtest" = xyes])

AC_SUBST([GTEST_LDFLAGS])
AC_SUBST([GTEST_CXXFLAGS])

# Enable coverage reports using gcov
AC_TDD_GCOV

AM_CONDITIONAL([HAVE_GCOV], [test "x$ac_cv_check_gcov" = xyes])
AS_IF([test "x$ac_cv_check_gcov" = xyes],
      [AC_CHECK_PROG([SUDO], [sudo], [sudo])
       AS_IF([test "x$ac_cv_prog_SUDO" = x],
             [AC_MSG_WARN([sudo is not installed, coverage reports may fail])])])

AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 include/oif/frame.h
                 src/Makefile
                 test/Makefile
                 test/gtest/Makefile
                 test/integration/Makefile
                 test/regular/Makefile
                 test/x11_mocks/Makefile
                 tools/Makefile
                 frame.pc])

AS_IF([test "x$have_xinput" = "xyes"],
      [AC_CONFIG_FILES([frame-x11.pc])])

AC_LANG_PUSH(C++)
AC_CHECK_DECL(__clang__, CXX_LIBS="-lstdc++")
AC_SUBST(CXX_LIBS)
AC_LANG_POP(C++)

AC_SUBST(WARNING_CPPFLAGS, "-Wall -Werror")
AC_SUBST(C11_CFLAGS, "-std=c1x")
AC_SUBST(CXX11_CXXFLAGS, "-std=c++0x")
AC_SUBST(VISIBILITY_CFLAGS, "-fvisibility=hidden")
AC_SUBST(VISIBILITY_CXXFLAGS, "-fvisibility=hidden")

AC_OUTPUT

bold_green=$(tput bold)$(tput setf 2)
bold_white=$(tput bold)$(tput setf 7)
reset=$(tput sgr0)

AC_MSG_NOTICE([])
AC_MSG_NOTICE([${bold_green}Open Input Framework - Frame library - $VERSION${reset}])
AC_MSG_NOTICE([])
AC_MSG_NOTICE([   Prefix                  : ${bold_white}${prefix}${reset}])
AC_MSG_NOTICE([   CXXFLAGS                : ${bold_white}${CXXFLAGS}${reset}])
AC_MSG_NOTICE([   Build X11 backend       : ${bold_white}${have_xinput}${reset}])
AC_MSG_NOTICE([   Build integration tests : ${bold_white}${have_xorg_gtest}${reset}])
AC_MSG_NOTICE([])
