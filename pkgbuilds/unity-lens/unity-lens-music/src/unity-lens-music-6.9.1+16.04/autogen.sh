#!/bin/sh

srcdir=`dirname $0`

PKG_NAME="unity-place-music"

which gnome-autogen.sh || {
	echo "You need gnome-common from GNOME SVN"
	exit 1
}

USE_GNOME2_MACROS=1 \
. gnome-autogen.sh
