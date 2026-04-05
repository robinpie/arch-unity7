file(REMOVE_RECURSE
  "../../generated/bench_options.cpp"
  "../../generated/bench_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/bench-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
