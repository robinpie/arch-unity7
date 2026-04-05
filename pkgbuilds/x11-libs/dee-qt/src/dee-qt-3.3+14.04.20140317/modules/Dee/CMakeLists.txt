
if (WITHQT5)
  find_package(Qt5Quick REQUIRED)
  set(OUR_QT_QUICK_LIB ${Qt5Quick_LIBRARIES})

  get_target_property(QMAKE_EXECUTABLE Qt5::qmake LOCATION)
  exec_program(${QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_QML" OUTPUT_VARIABLE QT_IMPORTS_DIR)
  file(TO_CMAKE_PATH "${QT_IMPORTS_DIR}" QT_IMPORTS_DIR)

  set(OUR_QT_QUICK_INCLUDE ${Qt5Quick_INCLUDE_DIRS})
  LIST(APPEND QT_PKGCONFIG_DEPENDENCIES "Qt5Quick")
else ()
  set(OUR_QT_QUICK_LIB ${QT_QTDECLARATIVE_LIBRARIES})
  set(OUR_QT_QUICK_INCLUDE ${QT_QTDECLARATIVE_INCLUDE_DIR})
  LIST(APPEND QT_PKGCONFIG_DEPENDENCIES "QtDeclarative")
endif ()

set(DeePlugin_SRCS
    plugin.cpp
    )

add_library(DeePlugin SHARED ${DeePlugin_SRCS})

target_link_libraries(DeePlugin
  ${DEE_QT_LIBNAME}
  ${OUR_QT_QUICK_LIB}
  )

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${OUR_QT_QUICK_INCLUDE}
    )

# copy qmldir file into build directory for shadow builds
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/qmldir"
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    )

install(TARGETS DeePlugin
    LIBRARY DESTINATION ${QT_IMPORTS_DIR}/Dee.${SONAME}
    )

install(FILES qmldir
    DESTINATION ${QT_IMPORTS_DIR}/Dee.${SONAME}
    )
