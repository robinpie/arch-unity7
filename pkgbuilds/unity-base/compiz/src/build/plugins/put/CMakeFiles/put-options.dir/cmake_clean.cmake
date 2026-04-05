file(REMOVE_RECURSE
  "../../generated/put_options.cpp"
  "../../generated/put_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/put-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
