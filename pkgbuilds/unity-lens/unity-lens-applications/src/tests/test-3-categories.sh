#!/bin/sh

# Do empty search, print the results, look at category id field, make sure
# there are exactly 3 different values for it
unity-tool -n com.canonical.Unity.Lens.Applications -p /com/canonical/unity/lens/applications -s "" --no-search-reply -r | awk -F "\t" ' { print $3; } ' | uniq | wc -l | grep -q 3 || exit 1

