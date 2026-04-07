#!/bin/sh

# Search for "ls", print the results, this search should only include both
# "ls", not "ls.+"
test `unity-tool -n com.canonical.Unity.Lens.Applications -p /com/canonical/unity/lens/commands -s "ls" --no-search-reply -r | grep "\\<ls\\>" | wc -l` -eq 1 || exit 1
test `unity-tool -n com.canonical.Unity.Lens.Applications -p /com/canonical/unity/lens/commands -s "ls" --no-search-reply -r | grep "\\<ls" | wc -l` -ge 2 || exit 1

