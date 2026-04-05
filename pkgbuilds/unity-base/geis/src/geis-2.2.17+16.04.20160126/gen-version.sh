#!/bin/sh
#
# Tool to automatically generate the package version from the bzr tags.
#
# This tool generates a package version string based on the tags in the bzr
# branch.  When an official release is made, the branch should be tagged with a
# tag of the form 'vN.N.N'.  This script will then give a version string of
# 'N.N.N'.  If any changes are made to the barcnh after tagging, the version
# string will be of the form 'N.N.N+rRRR' where RRR is the bzr revno of the
# latest commit.
#
# This scheme allows snapshot releases to be packaged.
#

# Copyright 2011 Canonical Ltd.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of version 3 of the GNU General Public License as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Default to some unknown version string.
version_string="unknown"

# Get the cirrent BZR revision number.
cur_bzr_rev=$(bzr version-info --custom --template="{revno}")

# Extract the release version string and BZR revision number.
tag_line=$(bzr tags --sort=time \
         | grep -E 'v[[:digit:]]+\.[[:digit:]]+\.[[:digit:]]+' \
         | tail -1)
tag_version=$(echo $tag_line | sed -n -e 's/v\([^[:space:]]*\).*/\1/p')
tag_rev=$(echo $tag_line | cut -d' ' -f2-)

if [ -n "$tag_version" ]; then
  version_string=$tag_version
fi

if [ $cur_bzr_rev -gt $tag_rev ]; then
  version_string="${version_string}+r${cur_bzr_rev}"
fi

echo $version_string
