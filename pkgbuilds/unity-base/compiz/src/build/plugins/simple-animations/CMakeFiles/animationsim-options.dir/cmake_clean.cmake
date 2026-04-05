file(REMOVE_RECURSE
  "../../generated/animationsim_options.cpp"
  "../../generated/animationsim_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/animationsim-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
