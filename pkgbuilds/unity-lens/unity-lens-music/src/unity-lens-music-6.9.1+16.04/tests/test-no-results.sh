#!/bin/sh

# Check that a non-sensical search string returns reply with "no-results-hint"
# set.
unity-tool -n com.canonical.Unity.Lens.Music -p /com/canonical/unity/lens/music -s asdfqwer | grep -q "no-results-hint" || exit 1

