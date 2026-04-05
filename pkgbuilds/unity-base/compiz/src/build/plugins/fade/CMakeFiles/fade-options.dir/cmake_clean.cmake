file(REMOVE_RECURSE
  "../../generated/fade_options.cpp"
  "../../generated/fade_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/fade-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
