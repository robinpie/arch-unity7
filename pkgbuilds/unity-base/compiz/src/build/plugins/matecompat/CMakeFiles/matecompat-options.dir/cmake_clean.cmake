file(REMOVE_RECURSE
  "../../generated/matecompat_options.cpp"
  "../../generated/matecompat_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/matecompat-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
