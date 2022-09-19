find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Widgets PrintSupport REQUIRED
  )
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Widgets PrintSupport REQUIRED
  )

set(QCUSTOMPLOT_SRC_DIR ${CMAKE_SOURCE_DIR}/../include/qcustomplot)

### MAIN PART

set(QCUSTOMPLOT_BINARY_DIR ${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}/bin)
set(QCUSTOMPLOT_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}/include)
set(QCUSTOMPLOT_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}/lib)

if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
  set(CHANGING_FILE ${QCUSTOMPLOT_LIBRARY_DIR}/qcustomplot.lib)
else()
  set(CHANGING_FILE ${QCUSTOMPLOT_LIBRARY_DIR}/libqcustomplot.so)
endif()

ExternalProject_Add(
  QCUSTOMPLOTBuild
  PREFIX QCUSTOMPLOT
  SOURCE_DIR ${QCUSTOMPLOT_SRC_DIR}
  CONFIGURE_COMMAND
    ${CMAKE_COMMAND} ${CUSTOM_TARGET_GENERATOR} ${CUSTOM_TARGET_PLATFORM_ARG} -S ${QCUSTOMPLOT_SRC_DIR}
    -DQT${QT_VERSION_MAJOR}_DIR:STRING=${QT_DIR}
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
get_filename_component(_fullpath "${QCUSTOMPLOT_BINARY_DIR}" REALPATH)
if(NOT EXISTS ${_fullpath})
  file(MAKE_DIRECTORY ${_fullpath})
  message(STATUS "Created directory: " ${_fullpath})
endif()
get_filename_component(_fullpath "${QCUSTOMPLOT_INCLUDE_DIRS}" REALPATH)
if(NOT EXISTS ${_fullpath})
  file(MAKE_DIRECTORY ${_fullpath})
  message(STATUS "Created directory: " ${_fullpath})
endif()
get_filename_component(_fullpath "${QCUSTOMPLOT_LIBRARY_DIR}" REALPATH)
if(NOT EXISTS ${_fullpath})
  file(MAKE_DIRECTORY ${_fullpath})
  message(STATUS "Created directory: " ${_fullpath})
endif()

add_library(QCUSTOMPLOT SHARED IMPORTED GLOBAL)
set_target_properties(QCUSTOMPLOT PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
                                            ${QCUSTOMPLOT_INCLUDE_DIRS})
if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
  set_target_properties(
    QCUSTOMPLOT PROPERTIES IMPORTED_LOCATION ${QCUSTOMPLOT_BINARY_DIR}
               IMPORTED_IMPLIB ${QCUSTOMPLOT_LIBRARY_DIR}/qcustomplot.lib)
else()
  set_target_properties(
    QCUSTOMPLOT PROPERTIES IMPORTED_LOCATION
                          ${QCUSTOMPLOT_LIBRARY_DIR}/libqcustomplot.so)
endif()

set(QCUSTOMPLOT_LIBS QCUSTOMPLOT)
set(QCUSTOMPLOT_LIBRARY_DIRS ${QCUSTOMPLOT_BINARY_DIR} ${QCUSTOMPLOT_LIBRARY_DIR})

add_dependencies(QCUSTOMPLOT QCUSTOMPLOTBuild)
