file(REMOVE_RECURSE
  "../../generated/shift_options.cpp"
  "../../generated/shift_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/shift-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
