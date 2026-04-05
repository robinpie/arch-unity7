file(REMOVE_RECURSE
  "../../generated/rotate_options.cpp"
  "../../generated/rotate_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/rotate-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
