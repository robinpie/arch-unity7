#
# @file m4/ac_enable_xi2.m4
# @brief autoconf macro to enable or disable support for XInput 2.2
#
# Copyright 2011, 2013 Canonical, Ltd.
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranties of
# MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
# PURPOSE.  See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>.
#
AC_DEFUN([AX_ENABLE_XI2],[
  AC_ARG_ENABLE([xi2.1],
                AS_HELP_STRING([--disable-xi2.1], [Disable XI2.1 features]))
  AS_IF([test "x$enable_xi2_1" != "xno"],[
    AC_MSG_CHECKING([for XI2.1])
    ax_have_xi_2_1=no
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([
      #include <X11/extensions/XInput2.h>
      XITouchClassInfo* p = 0;
      ])],
      [ax_have_xi_2_1=yes
       AC_DEFINE([HAVE_XI_2_1],[1],[XInput 2.1 is available])]
    )
    AC_MSG_RESULT([$ax_have_xi_2_1])
    PKG_CHECK_MODULES([XORG], [xorg-server >= 1.10.1], ,
        AC_MSG_ERROR([X.Org Server development libraries not found]))
  ])
])

