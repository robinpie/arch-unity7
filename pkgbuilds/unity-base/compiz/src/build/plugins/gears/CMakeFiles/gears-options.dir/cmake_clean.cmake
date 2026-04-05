file(REMOVE_RECURSE
  "../../generated/gears_options.cpp"
  "../../generated/gears_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/gears-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
