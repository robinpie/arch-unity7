file(REMOVE_RECURSE
  "../../generated/resize_options.cpp"
  "../../generated/resize_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/resize-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
