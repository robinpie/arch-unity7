file(REMOVE_RECURSE
  "../../generated/gnomecompat_options.cpp"
  "../../generated/gnomecompat_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/gnomecompat-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
