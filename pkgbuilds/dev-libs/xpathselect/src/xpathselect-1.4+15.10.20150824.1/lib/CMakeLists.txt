FIND_PACKAGE( Boost 1.40 REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wl,--no-undefined")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG" )
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
set(SOURCES node.cpp xpathselect.h xpathselect.cpp parser.cpp parser.h)
set(HEADERS node.h xpathselect.h)

if(CMAKE_COMPILER_IS_GNUCXX)
   execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
   if (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
        message(STATUS "C++11 activated.")
        add_definitions("-std=c++11")
   elseif(GCC_VERSION VERSION_GREATER 4.3 OR GCC_VERSION VERSION_EQUAL 4.3)
        message(WARNING "C++0x activated. If you get any errors update to a compiler which fully supports C++11")
        add_definitions("-std=c++0x")
   else ()
        message(FATAL_ERROR "C++11 needed. Therefore a gcc compiler with a version higher than 4.3 is needed.")
   endif()
else(CMAKE_COMPILER_IS_GNUCXX)
   add_definitions("-std=c++11") #turn on and hope non-gnu compiler supports it
endif(CMAKE_COMPILER_IS_GNUCXX)

add_library(xpathselect SHARED ${SOURCES})
target_link_libraries(xpathselect ${Boost_LIBRARIES})

set_target_properties(xpathselect
  PROPERTIES
  VERSION ${VERSION}
  SOVERSION ${VERSION})

install(TARGETS xpathselect
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES ${HEADERS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

configure_file(xpathselect.pc.in
    xpathselect.pc
    @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/lib/xpathselect.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
