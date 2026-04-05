file(REMOVE_RECURSE
  "libunity-api.a"
  "libunity-api.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/unity-api-static.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
