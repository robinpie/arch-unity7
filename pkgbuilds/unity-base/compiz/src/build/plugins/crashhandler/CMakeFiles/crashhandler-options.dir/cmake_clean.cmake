file(REMOVE_RECURSE
  "../../generated/crashhandler_options.cpp"
  "../../generated/crashhandler_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/crashhandler-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
