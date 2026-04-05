file(REMOVE_RECURSE
  "../../generated/session_options.cpp"
  "../../generated/session_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/session-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
