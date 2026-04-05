file(REMOVE_RECURSE
  "../../generated/workspacenames_options.cpp"
  "../../generated/workspacenames_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/workspacenames-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
