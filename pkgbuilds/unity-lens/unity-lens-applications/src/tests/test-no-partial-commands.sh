#!/bin/sh

# Search for "modpro", print the results, this search should only include
# "modprobe", not "modpro" itself
test `unity-tool -n com.canonical.Unity.Lens.Applications -p /com/canonical/unity/lens/commands -s "modpro" --no-search-reply -r | grep "modpro" | wc -l` -eq `unity-tool -n com.canonical.Unity.Lens.Applications -p /com/canonical/unity/lens/commands -s "modpro" --no-search-reply -r | grep "modprobe" | wc -l` || exit 1

