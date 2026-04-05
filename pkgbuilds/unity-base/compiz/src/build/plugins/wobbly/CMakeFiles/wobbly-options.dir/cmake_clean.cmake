file(REMOVE_RECURSE
  "../../generated/wobbly_options.cpp"
  "../../generated/wobbly_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/wobbly-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
