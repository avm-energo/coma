
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
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -S ${QXLSX_SRC_DIR} -DQt${QT_VERSION_MAJOR}_DIR:STRING=${QT_DIR} -DCMAKE_BUILD_TYPE:String=Release -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_SOURCE_DIR}
    BUILD_COMMAND ${CMAKE_COMMAND} --build . --config Release
    INSTALL_COMMAND cmake --install . --config Release
    USES_TERMINAL_BUILD TRUE
    USES_TERMINAL_CONFIGURE TRUE)

set(QXLSX_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(QXLSX_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include)
set(QXLSX_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/lib)

add_library(qxlsx SHARED IMPORTED GLOBAL)
set_target_properties(qxlsx PROPERTIES
  IMPORTED_LOCATION ${QXLSX_BINARY_DIR}
  INTERFACE_INCLUDE_DIRECTORIES ${QXLSX_INCLUDE_DIRS}
  IMPORTED_IMPLIB ${QXLSX_LIBRARY_DIR}/qxlsx.lib)

include_directories(${QXLSX_INCLUDE_DIRS})



set(QXLSX_LIBS qxlsx)
set(QXLSX_LIBRARY_DIRS ${QXLSX_BINARY_DIR} ${QXLSX_LIBRARY_DIR})

link_directories(${QXLSX_LIBRARY_DIRS})

# verify that the TBB header files can be included
set(CMAKE_REQUIRED_INCLUDES_SAVE ${CMAKE_REQUIRED_INCLUDES})
set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} 	${QXLSX_INCLUDE_DIRS})

# TODO FIX false negative
check_include_file_cxx("QXlsx/xlsxdocument.h" HAVE_QXLSX)
set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES_SAVE})
 if (NOT HAVE_QXLSX)
        message(STATUS "Did not build QXlsx correctly as cannot find xlsxdocument.h. Will build it.")
        set(HAVE_QXLSX 1)
endif (NOT HAVE_QXLSX)

#set(QXlsx_DIR ${CMAKE_CURRENT_LIST_DIR}/../share/QXlsx/cmake)
