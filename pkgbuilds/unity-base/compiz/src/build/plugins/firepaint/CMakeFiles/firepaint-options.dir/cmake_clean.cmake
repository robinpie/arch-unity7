file(REMOVE_RECURSE
  "../../generated/firepaint_options.cpp"
  "../../generated/firepaint_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/firepaint-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
