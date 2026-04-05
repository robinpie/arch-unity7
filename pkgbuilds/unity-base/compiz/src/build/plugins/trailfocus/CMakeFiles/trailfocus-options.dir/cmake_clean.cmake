file(REMOVE_RECURSE
  "../../generated/trailfocus_options.cpp"
  "../../generated/trailfocus_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/trailfocus-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
