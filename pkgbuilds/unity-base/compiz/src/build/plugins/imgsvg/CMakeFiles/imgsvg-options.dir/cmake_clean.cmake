file(REMOVE_RECURSE
  "../../generated/imgsvg_options.cpp"
  "../../generated/imgsvg_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/imgsvg-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
