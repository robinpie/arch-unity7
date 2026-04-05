file(REMOVE_RECURSE
  "../../generated/widget_options.cpp"
  "../../generated/widget_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/widget-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
