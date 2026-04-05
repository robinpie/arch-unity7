file(REMOVE_RECURSE
  "../../generated/neg_options.cpp"
  "../../generated/neg_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/neg-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
