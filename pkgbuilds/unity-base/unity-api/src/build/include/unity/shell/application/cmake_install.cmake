# Install script for directory: /run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity-api/src/include/unity/shell/application

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
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
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

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/unity/shell/application" TYPE FILE FILES
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity-api/src/include/unity/shell/application/ApplicationInfoInterface.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity-api/src/include/unity/shell/application/ApplicationManagerInterface.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity-api/src/include/unity/shell/application/Mir.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity-api/src/include/unity/shell/application/MirMousePointerInterface.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity-api/src/include/unity/shell/application/MirPlatformCursor.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity-api/src/include/unity/shell/application/MirSurfaceInterface.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity-api/src/include/unity/shell/application/MirSurfaceItemInterface.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity-api/src/include/unity/shell/application/MirSurfaceListInterface.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity-api/src/include/unity/shell/application/SurfaceManagerInterface.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity-api/src/build/data/unity-shell-application.pc")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity-api/src/build/include/unity/shell/application/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
