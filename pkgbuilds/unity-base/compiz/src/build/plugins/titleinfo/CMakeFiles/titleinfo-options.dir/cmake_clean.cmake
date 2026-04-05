file(REMOVE_RECURSE
  "../../generated/titleinfo_options.cpp"
  "../../generated/titleinfo_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/titleinfo-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
