file(REMOVE_RECURSE
  "../../generated/scale_options.cpp"
  "../../generated/scale_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/scale-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
