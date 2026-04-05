file(REMOVE_RECURSE
  "../../generated/grid_options.cpp"
  "../../generated/grid_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/grid-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
