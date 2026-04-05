file(REMOVE_RECURSE
  "../../generated/snap_options.cpp"
  "../../generated/snap_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/snap-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
