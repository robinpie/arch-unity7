file(REMOVE_RECURSE
  "../../generated/expo_options.cpp"
  "../../generated/expo_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/expo-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
