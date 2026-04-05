file(REMOVE_RECURSE
  "../../generated/animation_options.cpp"
  "../../generated/animation_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/animation-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
