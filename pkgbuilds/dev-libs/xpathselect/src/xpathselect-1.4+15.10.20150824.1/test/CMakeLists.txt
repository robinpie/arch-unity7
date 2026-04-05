
include_directories(../lib)

find_path(GTEST_SRC_DIR gtest.cc PATHS /usr/src/gtest/src)

set(CMAKE_CXX_FLAGS "-ggdb -O0 -Wall")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG" )
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

# Detect if C++11 support is available, and attempt fallback to C++0x support
if(CMAKE_COMPILER_IS_GNUCXX)
   execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
   if (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
        message(STATUS "C++11 activated.")
        add_definitions("-std=c++11")

        #run all tests
        set(SOURCES test_main.cpp test_xpath_simple.cpp test_xpath_tree.cpp dummynode.h test_parser.cpp)
        add_executable(test-runner ${SOURCES})

        add_dependencies (test-runner gtest)
        add_dependencies (test-runner ../lib)
        target_link_libraries(test-runner gtest xpathselect)

   elseif(GCC_VERSION VERSION_GREATER 4.3 OR GCC_VERSION VERSION_EQUAL 4.3)
        message(WARNING "C++0x activated. Tests have been disabled")
        add_definitions("-std=c++0x")
   else ()
        message(FATAL_ERROR "C++11 needed. Therefore a gcc compiler with a version higher than 4.3 is needed.")
   endif()
else(CMAKE_COMPILER_IS_GNUCXX)
   add_definitions("-std=c++11") #turn on and hope non-gnu compiler supports it
endif(CMAKE_COMPILER_IS_GNUCXX)

# Check for google test and build it locally
set(
  GTEST_ROOT_DIR
  "/usr/src/gtest" # Default value, adjustable by user with e.g., ccmake
  CACHE
  PATH
  "Path to Google test srcs"
)

find_path(GTEST_INCLUDE_DIR gtest/gtest.h)
if (GTEST_INCLUDE_DIR)
  #FIXME - hardcoded is bad!
  add_subdirectory(
    ${GTEST_ROOT_DIR}
    gtest
  )
endif(GTEST_INCLUDE_DIR)

enable_testing()
add_test(TestSuite test-runner)
add_custom_target (test COMMAND ./test-runner)
