file(REMOVE_RECURSE
  "../../generated/obs_options.cpp"
  "../../generated/obs_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/obs-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
