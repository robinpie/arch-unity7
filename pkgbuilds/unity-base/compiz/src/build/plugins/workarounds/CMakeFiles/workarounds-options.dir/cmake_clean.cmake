file(REMOVE_RECURSE
  "../../generated/workarounds_options.cpp"
  "../../generated/workarounds_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/workarounds-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
