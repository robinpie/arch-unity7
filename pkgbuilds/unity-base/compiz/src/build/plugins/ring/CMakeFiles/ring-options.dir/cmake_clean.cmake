file(REMOVE_RECURSE
  "../../generated/ring_options.cpp"
  "../../generated/ring_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/ring-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
