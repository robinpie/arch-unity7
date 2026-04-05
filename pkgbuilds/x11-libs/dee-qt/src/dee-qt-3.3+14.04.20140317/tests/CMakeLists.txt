if (WITHQT5)
  find_package(Qt5Test REQUIRED)
  set(OUR_QT_TEST_LIB ${Qt5Test_LIBRARIES})
  set(OUR_QT_TEST_INCLUDES ${Qt5Test_INCLUDE_DIRS})
else ()
  set(OUR_QT_TEST_LIB ${QT_QTTEST_LIBRARIES})
  set(OUR_QT_TEST_INCLUDES ${QT_QTTEST_INCLUDE_DIR})
endif ()

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${OUR_QT_TEST_INCLUDES}
    )

add_executable(conversiontest conversiontest.cpp)
target_link_libraries(conversiontest ${OUR_QT_TEST_LIB} ${DEE_QT_LIBNAME})
set_target_properties(conversiontest PROPERTIES COMPILE_FLAGS -fPIC)
add_test(NAME conversiontest COMMAND "${CMAKE_CURRENT_BINARY_DIR}/conversiontest" "-o" "${CMAKE_CURRENT_BINARY_DIR}/conversiontest-xunit.xml,xunitxml" "-o" "-,txt")
set_property(TEST conversiontest PROPERTY ENVIRONMENT "LD_LIBRARY_PATH=.")

if (WITHQT5)
	# the test is using signal connections with lambdas, only available in Qt5
	add_executable(signaltest signaltest.cpp)
	target_link_libraries(signaltest ${OUR_QT_TEST_LIB} ${DEE_QT_LIBNAME})
	set_target_properties(signaltest PROPERTIES COMPILE_FLAGS -fPIC)
	add_test(NAME signaltest COMMAND "${CMAKE_CURRENT_BINARY_DIR}/signaltest" "-o" "${CMAKE_CURRENT_BINARY_DIR}/signaltest-xunit.xml,xunitxml" "-o" "-,txt")
	set_property(TEST signaltest PROPERTY ENVIRONMENT "LD_LIBRARY_PATH=.")
endif ()

add_executable(test-helper test-helper.cpp)
target_link_libraries(test-helper ${OUR_QT_CORE_LIB} ${OUR_QT_DBUS_LIB} ${DEE_LDFLAGS})
set_target_properties(test-helper PROPERTIES COMPILE_FLAGS -fPIC)

add_executable(deelistmodeltest deelistmodeltest.cpp)
target_link_libraries(deelistmodeltest ${OUR_QT_TEST_LIB} ${OUR_QT_DBUS_LIB} ${DEE_QT_LIBNAME})
set_target_properties(deelistmodeltest PROPERTIES COMPILE_FLAGS -fPIC)
add_test(NAME deelistmodeltest COMMAND "dbus-test-runner" "--task" "${CMAKE_CURRENT_BINARY_DIR}/test-helper" "--task" "${CMAKE_CURRENT_BINARY_DIR}/deelistmodeltest" "-p" "-xunitxml" "-p" "-o" "-p" "deelistmodeltest-xunit.xml")
set_property(TEST deelistmodeltest PROPERTY ENVIRONMENT "LD_LIBRARY_PATH=.")
