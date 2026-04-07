#!/bin/sh
PROJECT="session-shortcuts"	# project name
WDIR=`pwd`		# working dir

echo "Preparing rc file"

echo 'i18nc("NAME OF TRANSLATORS","Your names");' >> ${WDIR}/rc.cpp
echo 'i18nc("EMAIL OF TRANSLATORS","Your emails");' >> ${WDIR}/rc.cpp

files=`find . -name '*.desktop.in'`

for desktop in $files; do
	intltool-extract --quiet --type=gettext/ini $desktop
	cat $desktop.h >> ${WDIR}/rc.cpp
	rm $desktop.h
done
echo "Done preparing rc file"

echo "Extracting messages"
xgettext -kN_:1 -o po/${PROJECT}.pot rc.cpp || { echo "error while calling xgettext. aborting."; exit 1; }
echo "Done extracting messages"

echo "Merging translations"
mkdir -p build
for desktop in $files; do
	intltool-merge --quiet --desktop-style po/ $desktop build/${desktop%.in}
done
echo "Done merging translations"

echo "Cleaning up"
rm rc.cpp
echo "Done"
