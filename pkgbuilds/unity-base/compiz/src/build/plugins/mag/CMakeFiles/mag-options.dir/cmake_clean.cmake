file(REMOVE_RECURSE
  "../../generated/mag_options.cpp"
  "../../generated/mag_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/mag-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
