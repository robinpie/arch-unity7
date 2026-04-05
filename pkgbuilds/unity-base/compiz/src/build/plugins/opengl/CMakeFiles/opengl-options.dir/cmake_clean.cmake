file(REMOVE_RECURSE
  "../../generated/opengl_options.cpp"
  "../../generated/opengl_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/opengl-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
