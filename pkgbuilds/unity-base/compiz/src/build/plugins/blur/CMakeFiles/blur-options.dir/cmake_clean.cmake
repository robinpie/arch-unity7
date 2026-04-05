file(REMOVE_RECURSE
  "../../generated/blur_options.cpp"
  "../../generated/blur_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/blur-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
