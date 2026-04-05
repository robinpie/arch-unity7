file(REMOVE_RECURSE
  "../../generated/thumbnail_options.cpp"
  "../../generated/thumbnail_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/thumbnail-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
