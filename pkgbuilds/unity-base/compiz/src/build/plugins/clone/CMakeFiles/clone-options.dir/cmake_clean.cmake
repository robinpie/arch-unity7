file(REMOVE_RECURSE
  "../../generated/clone_options.cpp"
  "../../generated/clone_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/clone-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
