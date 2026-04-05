file(REMOVE_RECURSE
  "../../generated/mousepoll_options.cpp"
  "../../generated/mousepoll_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/mousepoll-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
