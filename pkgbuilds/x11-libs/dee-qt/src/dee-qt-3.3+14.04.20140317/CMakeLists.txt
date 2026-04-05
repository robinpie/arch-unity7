project(dee-qt)
cmake_minimum_required(VERSION 3.10)

set(SONAME 3)
set(VERSION 3.3)
set(SOVERSION 3.0.0)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Dependencies
include(FindPkgConfig)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")

include(Coverage)

if (WITHQT5)
  message("Building Qt5 version")

  find_package(Qt5Core REQUIRED)
  find_package(Qt5DBus REQUIRED)

  add_definitions(-DWITHQT5=1)

  set(DEE_QT_LIBNAME dee-qt5)
  set(OUR_QT_INCLUDES ${Qt5Core_INCLUDE_DIRS} ${Qt5DBus_INCLUDE_DIRS})
  set(OUR_QT_CORE_LIB ${Qt5Core_LIBRARIES})
  set(OUR_QT_DBUS_LIB ${Qt5DBus_LIBRARIES})
  set(QT_PKGCONFIG_DEPENDENCIES "Qt5Core")
else ()
  message("Building Qt4 version")

  find_package(Qt4 REQUIRED)

  add_definitions(-DWITHQT5=0)

  set(DEE_QT_LIBNAME dee-qt4)
  set(OUR_QT_INCLUDES ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTDBUS_INCLUDE_DIR})
  set(OUR_QT_CORE_LIB ${QT_QTCORE_LIBRARIES})
  set(OUR_QT_DBUS_LIB ${QT_QTDBUS_LIBRARIES})
  set(QT_PKGCONFIG_DEPENDENCIES "QtCore QtDBus")
endif ()

set(QT_DEE_PKGCONFIG_FILE lib${DEE_QT_LIBNAME}.pc)

pkg_check_modules(DEE REQUIRED dee-1.0>=1.2.7)

# Build flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -Wall -Wundef -std=c++0x")

# Sources
set(DEE_QT_SRCS
    deelistmodel.cpp
    variantconversions.cpp
    )

# Build
add_definitions(-DQT_NO_KEYWORDS)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${OUR_QT_INCLUDES}
    ${DEE_INCLUDE_DIRS}
    )

add_library(${DEE_QT_LIBNAME} SHARED ${DEE_QT_SRCS})
target_link_libraries(${DEE_QT_LIBNAME}
    ${OUR_QT_CORE_LIB}
    ${DEE_LDFLAGS}
    )

set_target_properties(${DEE_QT_LIBNAME} PROPERTIES
  SOVERSION ${SONAME}
  VERSION ${SOVERSION}
  )

# Unit-Test
enable_testing()

add_subdirectory(modules)
add_subdirectory(tests)

# Install
set(INCLUDE_INSTALL_DIR include/lib${DEE_QT_LIBNAME})

install(TARGETS ${DEE_QT_LIBNAME}
    LIBRARY DESTINATION ${CMAKE_LIBRARY_ARCHITECTURE}${LIB_SUFFIX}
    )

install(FILES deelistmodel.h
    DESTINATION ${INCLUDE_INSTALL_DIR}
    )

# PkgConfig file
set (PREFIX "${CMAKE_INSTALL_PREFIX}")
set (EXEC_PREFIX "${CMAKE_INSTALL_PREFIX}")

configure_file (libdee-qt.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/${QT_DEE_PKGCONFIG_FILE} @ONLY)

install(FILES  ${CMAKE_CURRENT_BINARY_DIR}/${QT_DEE_PKGCONFIG_FILE}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_LIBRARY_ARCHITECTURE}/pkgconfig
    )
