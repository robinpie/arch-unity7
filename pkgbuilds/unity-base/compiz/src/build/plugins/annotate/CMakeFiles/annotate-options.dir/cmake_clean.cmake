file(REMOVE_RECURSE
  "../../generated/annotate_options.cpp"
  "../../generated/annotate_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/annotate-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
