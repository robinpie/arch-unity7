file(REMOVE_RECURSE
  "../../generated/staticswitcher_options.cpp"
  "../../generated/staticswitcher_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/staticswitcher-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
