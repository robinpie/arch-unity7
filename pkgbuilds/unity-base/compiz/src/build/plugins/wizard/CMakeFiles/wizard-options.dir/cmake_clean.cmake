file(REMOVE_RECURSE
  "../../generated/wizard_options.cpp"
  "../../generated/wizard_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/wizard-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
