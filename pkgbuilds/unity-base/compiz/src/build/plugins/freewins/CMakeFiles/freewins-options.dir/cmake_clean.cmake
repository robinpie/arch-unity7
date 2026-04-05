file(REMOVE_RECURSE
  "../../generated/freewins_options.cpp"
  "../../generated/freewins_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/freewins-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
