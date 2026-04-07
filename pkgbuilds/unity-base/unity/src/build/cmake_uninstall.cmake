message (STATUS "Uninstalling")
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.unityshell.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.unityshell.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/plugins/unityshell"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.unityshell.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
if (NOT EXISTS "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/install_manifest.txt")
  message (FATAL_ERROR "Cannot find install manifest: \"/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/install_manifest.txt\"")
endif (NOT EXISTS "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/install_manifest.txt")

file (READ "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/install_manifest.txt" files)
string (REGEX REPLACE "\n" ";" files "${files}")
foreach (file ${files})
  message (STATUS "Uninstalling \"${file}\"")
  if (EXISTS "${file}")
    exec_program(
      "/usr/bin/cmake" ARGS "-E remove \"${file}\""
      OUTPUT_VARIABLE rm_out
      RETURN_VALUE rm_retval
      )
    if ("${rm_retval}" STREQUAL 0)
    else ("${rm_retval}" STREQUAL 0)
      message (FATAL_ERROR "Problem when removing \"${file}\"")
    endif ("${rm_retval}" STREQUAL 0)
  else (EXISTS "${file}")
    message (STATUS "File \"${file}\" does not exist.")
  endif (EXISTS "${file}")
endforeach (file)
