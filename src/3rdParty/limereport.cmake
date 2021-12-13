find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Widgets Sql Network Xml Svg Qml PrintSupport REQUIRED
  )
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Widgets Sql Network Xml Svg Qml PrintSupport REQUIRED
  )

set(LIMEREPORT_SRC_DIR ${CMAKE_SOURCE_DIR}/../include/LimeReport)
message(STATUS "LimeReport directory: " ${LIMEREPORT_SRC_DIR})

### CONAN PART ##

conan_cmake_configure(REQUIRES libpng/1.6.37 GENERATORS cmake_find_package)

conan_cmake_install(
  PATH_OR_REFERENCE
  .
  INSTALL_FOLDER
  ${CMAKE_BINARY_DIR}
  BUILD
  missing
  SETTINGS
  ${settings})

### MAIN PART

set(LIMEREPORT_BINARY_DIR ${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}/bin)
set(LIMEREPORT_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}/include)
set(LIMEREPORT_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}/lib)

if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
  set(CHANGING_FILE ${LIMEREPORT_LIBRARY_DIR}/limereport.lib)
else()
  set(CHANGING_FILE ${LIMEREPORT_LIBRARY_DIR}/liblimereport.so)
endif()

ExternalProject_Add(
  LimeReportBuild
  PREFIX LimeReport
  SOURCE_DIR ${LIMEREPORT_SRC_DIR}
  CONFIGURE_COMMAND
    ${CMAKE_COMMAND} ${CUSTOM_TARGET_GENERATOR} ${CUSTOM_TARGET_PLATFORM_ARG} -S ${LIMEREPORT_SRC_DIR}
    -DQT${QT_VERSION_MAJOR}_DIR:STRING=${QT_DIR}
    -DQT_DIR:STRING=${QT_DIR}
    -DCMAKE_PREFIX_PATH:STRING=${CMAKE_PREFIX_PATH}
    -DCMAKE_BUILD_TYPE:String=Release
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}
    -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
    -DCMAKE_MODULE_PATH=${CMAKE_BINARY_DIR}
  BUILD_COMMAND ${CMAKE_COMMAND} --build . --config Release
  INSTALL_COMMAND ${CMAKE_COMMAND} --install . --config Release
  BUILD_BYPRODUCTS ${CHANGING_FILE}
  USES_TERMINAL_BUILD TRUE
  USES_TERMINAL_CONFIGURE TRUE)

# Hack to make it work, otherwise INTERFACE_INCLUDE_DIRECTORIES will not be
# propagated
get_filename_component(_fullpath "${LIMEREPORT_BINARY_DIR}" REALPATH)
if(NOT EXISTS ${_fullpath})
  file(MAKE_DIRECTORY ${_fullpath})
  message(STATUS "Created directory: " ${_fullpath})
endif()
get_filename_component(_fullpath "${LIMEREPORT_INCLUDE_DIRS}" REALPATH)
if(NOT EXISTS ${_fullpath})
  file(MAKE_DIRECTORY ${_fullpath})
  message(STATUS "Created directory: " ${_fullpath})
endif()
get_filename_component(_fullpath "${LIMEREPORT_LIBRARY_DIR}" REALPATH)
if(NOT EXISTS ${_fullpath})
  file(MAKE_DIRECTORY ${_fullpath})
  message(STATUS "Created directory: " ${_fullpath})
endif()

add_library(limereport SHARED IMPORTED GLOBAL)
set_target_properties(limereport PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
                                            ${LIMEREPORT_INCLUDE_DIRS})
if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
  set_target_properties(
    limereport
    PROPERTIES IMPORTED_LOCATION ${LIMEREPORT_BINARY_DIR}
               IMPORTED_IMPLIB ${LIMEREPORT_LIBRARY_DIR}/limereport.lib)
else()
  set_target_properties(
    limereport PROPERTIES IMPORTED_LOCATION
                          ${LIMEREPORT_LIBRARY_DIR}/liblimereport.so)
endif()

set(LIMEREPORT_LIBS limereport)
set(LIMEREPORT_LIBRARY_DIRS ${LIMEREPORT_BINARY_DIR} ${LIMEREPORT_LIBRARY_DIR})

add_dependencies(limereport LimeReportBuild)
