file(REMOVE_RECURSE
  "../../generated/wall_options.cpp"
  "../../generated/wall_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/wall-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
