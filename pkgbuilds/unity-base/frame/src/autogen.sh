#! /bin/sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

ORIGDIR=`pwd`
cd $srcdir

autoreconf --force --install || exit 1
cd $ORIGDIR || exit $?
test -n "$NOCONFIGURE" || "$srcdir/configure" "$@"

