file(REMOVE_RECURSE
  "../../generated/move_options.cpp"
  "../../generated/move_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/move-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
