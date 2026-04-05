file(REMOVE_RECURSE
  "../../generated/water_options.cpp"
  "../../generated/water_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/water-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
