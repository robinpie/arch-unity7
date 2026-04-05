file(REMOVE_RECURSE
  "../../generated/place_options.cpp"
  "../../generated/place_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/place-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
