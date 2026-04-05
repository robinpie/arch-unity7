file(REMOVE_RECURSE
  "../../generated/animationplus_options.cpp"
  "../../generated/animationplus_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/animationplus-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
