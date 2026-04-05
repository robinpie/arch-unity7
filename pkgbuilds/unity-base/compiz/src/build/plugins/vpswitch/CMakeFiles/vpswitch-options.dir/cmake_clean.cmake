file(REMOVE_RECURSE
  "../../generated/vpswitch_options.cpp"
  "../../generated/vpswitch_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/vpswitch-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
