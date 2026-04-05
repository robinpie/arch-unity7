file(REMOVE_RECURSE
  "../../generated/cube_options.cpp"
  "../../generated/cube_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/cube-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
