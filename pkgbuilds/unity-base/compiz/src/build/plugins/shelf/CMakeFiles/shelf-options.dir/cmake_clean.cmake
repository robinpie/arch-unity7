file(REMOVE_RECURSE
  "../../generated/shelf_options.cpp"
  "../../generated/shelf_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/shelf-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
