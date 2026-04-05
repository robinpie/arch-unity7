file(REMOVE_RECURSE
  "../generated/core_options.cpp"
  "../generated/core_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/core-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
