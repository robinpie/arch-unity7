# Install script for directory: /run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/UnityCore/libunity-core-6.0.so.9.0.0"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/UnityCore/libunity-core-6.0.so.9"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libunity-core-6.0.so.9.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libunity-core-6.0.so.9"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/UnityCore/libunity-core-6.0.so")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/Unity-6.0/UnityCore" TYPE FILE FILES
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/ActionHandle.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/ApplicationPreview.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/AppmenuIndicator.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/Categories.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/Category.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/CheckOptionFilter.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/ConnectionManager.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/DBusIndicators.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/DesktopUtilities.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/Filter.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/Filters.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/GenericPreview.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/GLibDBusNameWatcher.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/GLibDBusProxy.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/GLibDBusServer.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/GLibSignal.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/GLibSignal-inl.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/GLibSource.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/GLibWrapper.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/GLibWrapper-inl.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/GnomeSessionManager.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/GSettingsScopes.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/Hud.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/IndicatorEntry.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/Indicator.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/Indicators.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/MiscUtils.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/MoviePreview.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/MultiRangeFilter.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/MusicPreview.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/Model.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/Model-inl.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/ModelIterator.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/ModelIterator-inl.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/ModelRowAdaptor.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/ModelRowAdaptor-inl.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/PaymentPreview.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/Preview.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/PreviewPlayer.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/RadioOptionFilter.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/RatingsFilter.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/Result.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/Results.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/Scope.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/ScopeData.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/Scopes.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/ScopeProxy.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/ScopeProxyInterface.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/SessionManager.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/SocialPreview.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/Track.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/Tracks.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/UWeakPtr.h"
    "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/unity/UnityCore/Variant.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/UnityCore/unity-core-6.0.pc")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/run/media/robin/robinScard/codingStuff/arch-unity7/pkgbuilds/unity-base/unity/src/build/UnityCore/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
