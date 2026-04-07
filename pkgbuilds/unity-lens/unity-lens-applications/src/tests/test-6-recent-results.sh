#!/bin/sh

# Do empty search, print the results, look at the number of items
# for the recent category, make sure there are max 6
test `unity-tool -n com.canonical.Unity.Lens.Applications -p /com/canonical/unity/lens/applications -s "" --no-search-reply -r | awk -F "\t" ' { print $3; } ' | grep 0 | wc -l` -le 6

