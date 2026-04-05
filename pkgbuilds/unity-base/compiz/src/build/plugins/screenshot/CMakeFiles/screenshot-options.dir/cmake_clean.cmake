file(REMOVE_RECURSE
  "../../generated/screenshot_options.cpp"
  "../../generated/screenshot_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/screenshot-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
