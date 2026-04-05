#
# CMake file for autopilot daemon & test app.

cmake_minimum_required(VERSION 3.10)
project(xpathselect)

include (GNUInstallDirs)

set (VERSION 1.4)

if(CMAKE_BUILD_TYPE STREQUAL "coverage")
  message("Building for coverage")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage" )
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage" )
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} --coverage" )
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage" )
endif(CMAKE_BUILD_TYPE STREQUAL "coverage")

INCLUDE(FindPkgConfig)

add_subdirectory(lib)

