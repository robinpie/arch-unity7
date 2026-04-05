file(REMOVE_RECURSE
  "../../generated/switcher_options.cpp"
  "../../generated/switcher_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/switcher-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
