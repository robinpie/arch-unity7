file(REMOVE_RECURSE
  "../../generated/notification_options.cpp"
  "../../generated/notification_options.h"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/notification-options.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
