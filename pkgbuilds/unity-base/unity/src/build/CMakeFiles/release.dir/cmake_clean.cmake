file(REMOVE_RECURSE
  "api-doc"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/release.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
