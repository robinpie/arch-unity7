# Install script for directory: /run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/compiz-0.9.14.2+25.10.20250930/plugins

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/addhelper/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/animation/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/animationaddon/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/animationjc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/animationplus/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/annotate/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/bench/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/blur/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/ccp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/clone/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/colorfilter/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/commands/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/compiztoolbox/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/composite/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/copytex/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/crashhandler/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/cube/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/cubeaddon/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/dbus/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/decor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/expo/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/extrawm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/ezoom/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/fade/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/fadedesktop/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/firepaint/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/freewins/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/gears/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/gnomecompat/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/grid/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/imgjpeg/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/imgpng/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/imgsvg/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/inotify/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/mag/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/matecompat/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/maximumize/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/mblur/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/mousepoll/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/move/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/neg/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/notification/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/obs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/opacify/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/opengl/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/place/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/put/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/regex/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/resize/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/resizeinfo/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/ring/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/rotate/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/scale/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/scaleaddon/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/scalefilter/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/screenshot/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/session/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/shelf/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/shift/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/showdesktop/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/showmouse/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/showrepaint/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/simple-animations/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/snap/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/splash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/staticswitcher/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/switcher/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/td/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/text/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/thumbnail/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/titleinfo/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/trailfocus/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/vpswitch/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/wall/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/wallpaper/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/water/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/widget/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/winrules/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/wizard/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/wobbly/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/workarounds/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/workspacenames/cmake_install.cmake")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/compiz/src/build/plugins/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
