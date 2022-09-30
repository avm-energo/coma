find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Xml Widgets
  REQUIRED)

set(QXLSX_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include/QXlsx)
message(STATUS "QXlsx directory: " ${QXLSX_SRC_DIR})

### PATCH PART ##

execute_process(
  COMMAND git apply ${CMAKE_CURRENT_LIST_DIR}/0001-fix-broken-iterators.patch
  WORKING_DIRECTORY ${QXLSX_SRC_DIR}
  RESULT_VARIABLE CMD_RESULT
  OUTPUT_FILE CMD_OUTPUT
  ERROR_VARIABLE CMD_ERROR)

### MAIN PART

set(QXLSX_BINARY_DIR ${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}/bin)
set(QXLSX_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}/include)
set(QXLSX_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}/lib)

if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
  set(CHANGING_FILE ${QXLSX_LIBRARY_DIR}/QXlsx.lib)
else()
  set(CHANGING_FILE ${QXLSX_LIBRARY_DIR}/libQXlsx.so)
endif()

ExternalProject_Add(
  QXlsxBuild
  PREFIX QXlsx
  SOURCE_DIR ${QXLSX_SRC_DIR}
  CONFIGURE_COMMAND
    ${CMAKE_COMMAND} ${CUSTOM_TARGET_GENERATOR} ${CUSTOM_TARGET_PLATFORM_ARG} -S ${QXLSX_SRC_DIR}
    -DQt${QT_VERSION_MAJOR}_DIR:STRING=${QT_DIR}
    -DCMAKE_BUILD_TYPE:String=Release
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}
    -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
  BUILD_COMMAND ${CMAKE_COMMAND} --build . --config Release
  INSTALL_COMMAND ${CMAKE_COMMAND} --install . --config Release
  BUILD_BYPRODUCTS ${CHANGING_FILE}
  USES_TERMINAL_BUILD TRUE
  USES_TERMINAL_CONFIGURE TRUE)

# Hack to make it work, otherwise INTERFACE_INCLUDE_DIRECTORIES will not be
# propagated
get_filename_component(_fullpath "${QXLSX_BINARY_DIR}" REALPATH)
if(NOT EXISTS ${_fullpath})
  file(MAKE_DIRECTORY ${_fullpath})
  message(STATUS "Created directory: " ${_fullpath})
endif()
get_filename_component(_fullpath "${QXLSX_INCLUDE_DIRS}" REALPATH)
if(NOT EXISTS ${_fullpath})
  file(MAKE_DIRECTORY ${_fullpath})
  message(STATUS "Created directory: " ${_fullpath})
endif()
get_filename_component(_fullpath "${QXLSX_LIBRARY_DIR}" REALPATH)
if(NOT EXISTS ${_fullpath})
  file(MAKE_DIRECTORY ${_fullpath})
  message(STATUS "Created directory: " ${_fullpath})
endif()

add_library(QXlsx SHARED IMPORTED GLOBAL)
set_target_properties(QXlsx PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
                                       ${QXLSX_INCLUDE_DIRS})
if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
  set_target_properties(
    QXlsx PROPERTIES IMPORTED_LOCATION ${QXLSX_BINARY_DIR}
                     IMPORTED_IMPLIB ${QXLSX_LIBRARY_DIR}/QXlsx.lib)
else()
  set_target_properties(QXlsx PROPERTIES IMPORTED_LOCATION
                                         ${QXLSX_LIBRARY_DIR}/libQXlsx.so)
endif()

set(QXLSX_LIBS QXlsx)
set(QXLSX_LIBRARY_DIRS ${QXLSX_BINARY_DIR} ${QXLSX_LIBRARY_DIR})

# verify that the TBB header files can be included
set(CMAKE_REQUIRED_INCLUDES_SAVE ${CMAKE_REQUIRED_INCLUDES})
set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${QXLSX_INCLUDE_DIRS})


set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES_SAVE})


add_dependencies(QXlsx QXlsxBuild)
