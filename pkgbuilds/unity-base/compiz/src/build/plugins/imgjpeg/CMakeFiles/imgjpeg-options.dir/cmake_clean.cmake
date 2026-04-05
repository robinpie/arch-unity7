file(REMOVE_RECURSE
  "../../generated/imgjpeg_options.cpp"
  "../../generated/imgjpeg_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/imgjpeg-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
