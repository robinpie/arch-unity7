file(REMOVE_RECURSE
  "../../generated/colorfilter_options.cpp"
  "../../generated/colorfilter_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/colorfilter-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
