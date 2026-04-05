file(REMOVE_RECURSE
  "../../generated/splash_options.cpp"
  "../../generated/splash_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/splash-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
