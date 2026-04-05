file(REMOVE_RECURSE
  "../../generated/showmouse_options.cpp"
  "../../generated/showmouse_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/showmouse-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
