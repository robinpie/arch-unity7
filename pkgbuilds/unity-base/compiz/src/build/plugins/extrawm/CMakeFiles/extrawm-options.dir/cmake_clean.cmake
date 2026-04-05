file(REMOVE_RECURSE
  "../../generated/extrawm_options.cpp"
  "../../generated/extrawm_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/extrawm-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
