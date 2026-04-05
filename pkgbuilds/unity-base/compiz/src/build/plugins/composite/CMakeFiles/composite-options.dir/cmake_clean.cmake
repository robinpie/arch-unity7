file(REMOVE_RECURSE
  "../../generated/composite_options.cpp"
  "../../generated/composite_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/composite-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
