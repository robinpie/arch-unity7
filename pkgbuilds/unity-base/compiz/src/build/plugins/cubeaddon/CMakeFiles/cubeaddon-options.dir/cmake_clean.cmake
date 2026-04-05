file(REMOVE_RECURSE
  "../../generated/cubeaddon_options.cpp"
  "../../generated/cubeaddon_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/cubeaddon-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
