file(REMOVE_RECURSE
  "../../generated/commands_options.cpp"
  "../../generated/commands_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/commands-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
