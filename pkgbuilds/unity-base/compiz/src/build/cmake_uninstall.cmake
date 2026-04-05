message (STATUS "Uninstalling")
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas//org.compiz.gwd.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas//org.compiz.gwd.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/gtk/window-decorator"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas//org.compiz.gwd.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.core.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.core.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/metadata"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.core.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script /usr/bin/python3.14 /run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/compiz-0.9.14.2+25.10.20250930/compizconfig/compizconfig-python/setup.py uninstall --prefix=/usr --version=0.9.14.2")
execute_process (COMMAND /usr/bin/python3.14 /run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/compiz-0.9.14.2+25.10.20250930/compizconfig/compizconfig-python/setup.py uninstall --prefix=/usr --version=0.9.14.2
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/compizconfig/compizconfig-python"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script /usr/bin/python3.14 /run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/compiz-0.9.14.2+25.10.20250930/compizconfig/compizconfig-python/setup.py uninstall --prefix=/usr --version=0.9.14.2 : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.integrated.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.integrated.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/compizconfig/integration/gnome/gsettings"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.integrated.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script /usr/bin/python3.14 /run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/compiz-0.9.14.2+25.10.20250930/compizconfig/ccsm/setup.py uninstall --prefix=/usr --version=0.9.14.2")
execute_process (COMMAND /usr/bin/python3.14 /run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/compiz-0.9.14.2+25.10.20250930/compizconfig/ccsm/setup.py uninstall --prefix=/usr --version=0.9.14.2
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/compiz-0.9.14.2+25.10.20250930/compizconfig/ccsm"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script /usr/bin/python3.14 /run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/compiz-0.9.14.2+25.10.20250930/compizconfig/ccsm/setup.py uninstall --prefix=/usr --version=0.9.14.2 : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas//org.compiz.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas//org.compiz.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/compizconfig/gsettings"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas//org.compiz.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.addhelper.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.addhelper.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/addhelper"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.addhelper.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.animation.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.animation.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/animation"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.animation.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.animationaddon.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.animationaddon.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/animationaddon"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.animationaddon.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.animationjc.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.animationjc.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/animationjc"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.animationjc.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.animationplus.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.animationplus.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/animationplus"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.animationplus.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.annotate.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.annotate.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/annotate"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.annotate.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.bench.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.bench.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/bench"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.bench.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.blur.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.blur.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/blur"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.blur.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.ccp.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.ccp.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/ccp"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.ccp.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.clone.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.clone.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/clone"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.clone.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.colorfilter.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.colorfilter.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/colorfilter"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.colorfilter.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.commands.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.commands.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/commands"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.commands.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.compiztoolbox.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.compiztoolbox.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/compiztoolbox"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.compiztoolbox.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.composite.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.composite.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/composite"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.composite.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.copytex.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.copytex.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/copytex"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.copytex.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.crashhandler.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.crashhandler.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/crashhandler"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.crashhandler.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.cube.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.cube.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/cube"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.cube.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.cubeaddon.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.cubeaddon.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/cubeaddon"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.cubeaddon.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.dbus.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.dbus.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/dbus"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.dbus.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.decor.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.decor.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/decor"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.decor.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.expo.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.expo.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/expo"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.expo.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.extrawm.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.extrawm.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/extrawm"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.extrawm.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.ezoom.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.ezoom.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/ezoom"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.ezoom.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.fade.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.fade.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/fade"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.fade.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.fadedesktop.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.fadedesktop.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/fadedesktop"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.fadedesktop.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.firepaint.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.firepaint.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/firepaint"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.firepaint.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.freewins.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.freewins.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/freewins"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.freewins.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.gears.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.gears.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/gears"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.gears.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.gnomecompat.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.gnomecompat.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/gnomecompat"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.gnomecompat.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.grid.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.grid.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/grid"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.grid.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.imgjpeg.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.imgjpeg.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/imgjpeg"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.imgjpeg.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.imgpng.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.imgpng.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/imgpng"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.imgpng.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.imgsvg.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.imgsvg.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/imgsvg"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.imgsvg.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.inotify.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.inotify.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/inotify"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.inotify.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.mag.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.mag.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/mag"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.mag.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.matecompat.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.matecompat.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/matecompat"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.matecompat.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.maximumize.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.maximumize.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/maximumize"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.maximumize.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.mblur.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.mblur.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/mblur"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.mblur.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.mousepoll.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.mousepoll.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/mousepoll"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.mousepoll.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.move.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.move.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/move"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.move.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.neg.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.neg.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/neg"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.neg.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.notification.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.notification.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/notification"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.notification.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.obs.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.obs.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/obs"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.obs.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.opacify.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.opacify.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/opacify"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.opacify.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.opengl.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.opengl.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/opengl"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.opengl.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.place.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.place.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/place"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.place.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.put.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.put.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/put"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.put.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.regex.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.regex.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/regex"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.regex.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.resize.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.resize.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/resize"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.resize.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.resizeinfo.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.resizeinfo.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/resizeinfo"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.resizeinfo.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.ring.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.ring.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/ring"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.ring.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.rotate.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.rotate.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/rotate"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.rotate.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.scale.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.scale.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/scale"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.scale.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.scaleaddon.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.scaleaddon.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/scaleaddon"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.scaleaddon.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.scalefilter.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.scalefilter.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/scalefilter"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.scalefilter.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.screenshot.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.screenshot.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/screenshot"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.screenshot.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.session.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.session.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/session"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.session.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.shelf.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.shelf.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/shelf"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.shelf.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.shift.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.shift.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/shift"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.shift.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.showdesktop.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.showdesktop.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/showdesktop"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.showdesktop.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.showmouse.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.showmouse.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/showmouse"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.showmouse.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.showrepaint.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.showrepaint.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/showrepaint"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.showrepaint.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.animationsim.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.animationsim.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/simple-animations"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.animationsim.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.snap.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.snap.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/snap"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.snap.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.splash.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.splash.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/splash"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.splash.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.staticswitcher.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.staticswitcher.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/staticswitcher"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.staticswitcher.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.switcher.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.switcher.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/switcher"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.switcher.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.td.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.td.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/td"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.td.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.text.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.text.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/text"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.text.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.thumbnail.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.thumbnail.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/thumbnail"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.thumbnail.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.titleinfo.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.titleinfo.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/titleinfo"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.titleinfo.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.trailfocus.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.trailfocus.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/trailfocus"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.trailfocus.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.vpswitch.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.vpswitch.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/vpswitch"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.vpswitch.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.wall.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.wall.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/wall"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.wall.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.wallpaper.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.wallpaper.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/wallpaper"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.wallpaper.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.water.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.water.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/water"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.water.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.widget.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.widget.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/widget"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.widget.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.winrules.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.winrules.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/winrules"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.winrules.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.wizard.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.wizard.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/wizard"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.wizard.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.wobbly.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.wobbly.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/wobbly"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.wobbly.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.workarounds.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.workarounds.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/workarounds"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.workarounds.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
message (STATUS "Executing custom uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.workspacenames.gschema.xml")
execute_process (COMMAND cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.workspacenames.gschema.xml
                 WORKING_DIRECTORY "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/workspacenames"
                 OUTPUT_VARIABLE cmd_output
                 RESULT_VARIABLE cmd_ret)
message ("${cmd_output}")
if (NOT "${cmd_ret}" STREQUAL 0)
    message (FATAL_ERROR "Problem executing uninstall script cmake -E remove -f /usr/share/glib-2.0/schemas/org.compiz.workspacenames.gschema.xml : ${cmd_ret}")
endif (NOT "${cmd_ret}" STREQUAL 0)
if (NOT EXISTS "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/install_manifest.txt")
  message (FATAL_ERROR "Cannot find install manifest: \"/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/install_manifest.txt\"")
endif (NOT EXISTS "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/install_manifest.txt")

file (READ "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/install_manifest.txt" files)
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
