file(REMOVE_RECURSE
  "../../generated/unityshell_options.cpp"
  "../../generated/unityshell_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/unityshell-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
