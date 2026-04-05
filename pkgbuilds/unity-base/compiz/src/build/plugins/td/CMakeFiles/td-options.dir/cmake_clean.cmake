file(REMOVE_RECURSE
  "../../generated/td_options.cpp"
  "../../generated/td_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/td-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
