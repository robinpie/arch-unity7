file(REMOVE_RECURSE
  "../../generated/animationjc_options.cpp"
  "../../generated/animationjc_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/animationjc-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
