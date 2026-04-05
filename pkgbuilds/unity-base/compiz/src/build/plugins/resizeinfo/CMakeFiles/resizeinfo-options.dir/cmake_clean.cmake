file(REMOVE_RECURSE
  "../../generated/resizeinfo_options.cpp"
  "../../generated/resizeinfo_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/resizeinfo-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
