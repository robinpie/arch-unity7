file(REMOVE_RECURSE
  "../../generated/animationaddon_options.cpp"
  "../../generated/animationaddon_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/animationaddon-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
