file(REMOVE_RECURSE
  "../../generated/compiztoolbox_options.cpp"
  "../../generated/compiztoolbox_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/compiztoolbox-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
