file(REMOVE_RECURSE
  "../../generated/winrules_options.cpp"
  "../../generated/winrules_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/winrules-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
