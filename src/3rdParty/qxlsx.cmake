
find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Xml Widgets
  REQUIRED)

include(conanbuildinfo)

message(STATUS "QMAKE : " ${Qt${QT_VERSION_MAJOR}Core_QMAKE_EXECUTABLE})

set(QXLSX_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include/QXlsx)
message(STATUS "Limereport directory: " ${QXLSX_DIR})

execute_process(COMMAND git apply ${CMAKE_CURRENT_LIST_DIR}/0001-fix-broken-iterators.patch
    WORKING_DIRECTORY ${QXLSX_SRC_DIR}
    RESULT_VARIABLE CMD_RESULT
       OUTPUT_FILE CMD_OUTPUT
       ERROR_VARIABLE CMD_ERROR)

ExternalProject_Add(QXlsxBuild
    SOURCE_DIR ${QXLSX_SRC_DIR}
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -DQt${QT_VERSION_MAJOR}_DIR:STRING=${QT_DIR} -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_SOURCE_DIR} ${QXLSX_SRC_DIR}
    USES_TERMINAL_BUILD TRUE
    USES_TERMINAL_CONFIGURE TRUE)

set(QXlsx_DIR ${CMAKE_CURRENT_LIST_DIR}/../share/QXlsx/cmake)
