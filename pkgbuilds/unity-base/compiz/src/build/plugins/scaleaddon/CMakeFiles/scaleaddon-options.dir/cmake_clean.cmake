file(REMOVE_RECURSE
  "../../generated/scaleaddon_options.cpp"
  "../../generated/scaleaddon_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/scaleaddon-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
