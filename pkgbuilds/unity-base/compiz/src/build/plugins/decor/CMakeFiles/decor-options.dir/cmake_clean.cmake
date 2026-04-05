file(REMOVE_RECURSE
  "../../generated/decor_options.cpp"
  "../../generated/decor_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/decor-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
