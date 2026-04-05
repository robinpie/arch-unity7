file(REMOVE_RECURSE
  "../../generated/opacify_options.cpp"
  "../../generated/opacify_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/opacify-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
