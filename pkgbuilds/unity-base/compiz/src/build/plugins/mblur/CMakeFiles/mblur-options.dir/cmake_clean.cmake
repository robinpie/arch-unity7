file(REMOVE_RECURSE
  "../../generated/mblur_options.cpp"
  "../../generated/mblur_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/mblur-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
