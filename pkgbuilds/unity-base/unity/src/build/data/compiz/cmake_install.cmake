# Install script for directory: /run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/data/compiz

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
    set(CMAKE_INSTALL_CONFIG_NAME "None")
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
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/etc/compizconfig/unity.conf;/etc/compizconfig/unity.ini;/etc/compizconfig/unity-lowgfx.ini")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/etc/compizconfig" TYPE FILE FILES
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/data/compiz/unity.conf"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/data/compiz/unity.ini"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/data/compiz/unity-lowgfx.ini"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/share/compizconfig/upgrades/com.canonical.unity.unity.01.upgrade;/usr/share/compizconfig/upgrades/com.canonical.unity.unity.02.upgrade;/usr/share/compizconfig/upgrades/com.canonical.unity.unity.03.upgrade;/usr/share/compizconfig/upgrades/com.canonical.unity.unity.04.upgrade;/usr/share/compizconfig/upgrades/com.canonical.unity.unity.05.upgrade;/usr/share/compizconfig/upgrades/com.canonical.unity.unity.06.upgrade;/usr/share/compizconfig/upgrades/com.canonical.unity.unity.07.upgrade;/usr/share/compizconfig/upgrades/com.canonical.unity.unity-lowgfx.01.upgrade")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/usr/share/compizconfig/upgrades" TYPE FILE FILES
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/data/compiz/upgrades/com.canonical.unity.unity.01.upgrade"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/data/compiz/upgrades/com.canonical.unity.unity.02.upgrade"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/data/compiz/upgrades/com.canonical.unity.unity.03.upgrade"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/data/compiz/upgrades/com.canonical.unity.unity.04.upgrade"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/data/compiz/upgrades/com.canonical.unity.unity.05.upgrade"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/data/compiz/upgrades/com.canonical.unity.unity.06.upgrade"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/data/compiz/upgrades/com.canonical.unity.unity.07.upgrade"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/data/compiz/upgrades/com.canonical.unity.unity-lowgfx.01.upgrade"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/data/compiz/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
