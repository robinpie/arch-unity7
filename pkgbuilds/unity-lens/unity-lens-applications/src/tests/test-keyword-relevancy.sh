#!/bin/sh

# This test is pretty flaky, feel free to remove if it starts failing... :(

# Search for display, print the results, make sure first is:
# gnome-display-panel.desktop
unity-tool -n com.canonical.Unity.Lens.Applications -p /com/canonical/unity/lens/applications -s "display" --no-search-reply -r | head -n1 | grep -q gnome-display-panel.desktop || exit 1

