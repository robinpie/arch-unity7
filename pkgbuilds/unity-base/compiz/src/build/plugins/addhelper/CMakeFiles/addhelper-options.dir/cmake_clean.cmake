file(REMOVE_RECURSE
  "../../generated/addhelper_options.cpp"
  "../../generated/addhelper_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/addhelper-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
