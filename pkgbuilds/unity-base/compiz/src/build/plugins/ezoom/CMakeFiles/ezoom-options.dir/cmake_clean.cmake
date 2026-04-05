file(REMOVE_RECURSE
  "../../generated/ezoom_options.cpp"
  "../../generated/ezoom_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/ezoom-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
