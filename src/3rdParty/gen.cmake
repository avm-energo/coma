find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core REQUIRED
  )

find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core REQUIRED
  )

set(GEN_SRC_DIR ${CMAKE_SOURCE_DIR}/../include/gen/src)
message(STATUS "GEN directory: " ${GEN_SRC_DIR})

### MAIN PART

set(GEN_BINARY_DIR ${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}/bin)
set(GEN_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}/include)
set(GEN_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}/lib)

message(STATUS "GEN Library directory: " ${GEN_LIBRARY_DIR})

if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
    set(CHANGING_FILE ${GEN_LIBRARY_DIR}/gen.lib)
else()
  set(CHANGING_FILE ${GEN_LIBRARY_DIR}/gen.so)
endif()

message(STATUS "Builds available: " ${CMAKE_CONFIGURATION_TYPES})
message(STATUS "Current build: " ${CMAKE_BUILD_TYPE})

ExternalProject_Add(
  GenBuild
  PREFIX gen
  SOURCE_DIR ${GEN_SRC_DIR}
  CONFIGURE_COMMAND
    ${CMAKE_COMMAND} ${CUSTOM_TARGET_GENERATOR} ${CUSTOM_TARGET_PLATFORM_ARG} -S ${GEN_SRC_DIR}
    -DQT${QT_VERSION_MAJOR}_DIR:STRING=${QT_DIR}
    -DQT_DIR:STRING=${QT_DIR}
    -DPROJECT_ARCHITECTURE:STRING=${PROJECT_ARCHITECTURE}
    -DPROJECT_TARGET_NAME:STRING=${PROJECT_TARGET_NAME}
    -DCMAKE_PREFIX_PATH:STRING=${CMAKE_PREFIX_PATH}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_SOURCE_DIR}/${PROJECT_TARGET_NAME}
    -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
    -DCMAKE_C_COMPILER_TARGET:STRING=${CMAKE_C_COMPILER_TARGET}
    -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_COMPILER_TARGET:STRING=${CMAKE_CXX_COMPILER_TARGET}
    -DCMAKE_MODULE_PATH=${CMAKE_BINARY_DIR}
  BUILD_COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE}
  INSTALL_COMMAND ${CMAKE_COMMAND} --install . --config ${CMAKE_BUILD_TYPE}
  BUILD_BYPRODUCTS ${CHANGING_FILE}
  USES_TERMINAL_BUILD TRUE
  USES_TERMINAL_CONFIGURE TRUE)

# Hack to make it work, otherwise INTERFACE_INCLUDE_DIRECTORIES will not be propagated
get_filename_component(_fullpath "${GEN_BINARY_DIR}" REALPATH)
if(NOT EXISTS ${_fullpath})
  file(MAKE_DIRECTORY ${_fullpath})
  message(STATUS "Created directory: " ${_fullpath})
endif()
get_filename_component(_fullpath "${GEN_INCLUDE_DIRS}" REALPATH)
if(NOT EXISTS ${_fullpath})
  file(MAKE_DIRECTORY ${_fullpath})
  message(STATUS "Created directory: " ${_fullpath})
endif()
get_filename_component(_fullpath "${GEN_LIBRARY_DIR}" REALPATH)
if(NOT EXISTS ${_fullpath})
  file(MAKE_DIRECTORY ${_fullpath})
  message(STATUS "Created directory: " ${_fullpath})
endif()

add_library(gen SHARED IMPORTED GLOBAL)
set_target_properties(gen PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
                                            ${GEN_INCLUDE_DIRS})
message(STATUS "CMAKE_SYSTEM_NAME_LOWER: " ${CMAKE_SYSTEM_NAME_LOWER})
if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
  set_target_properties(gen
    PROPERTIES IMPORTED_LOCATION ${GEN_BINARY_DIR}
               IMPORTED_IMPLIB ${GEN_LIBRARY_DIR}/gen.lib)
else()
  set_target_properties(gen
      PROPERTIES IMPORTED_LOCATION ${GEN_LIBRARY_DIR}/libgen.so)
endif()

set(GEN_LIBS gen)
set(GEN_LIBRARY_DIRS ${GEN_BINARY_DIR} ${GEN_LIBRARY_DIR})

add_dependencies(gen GenBuild)

if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
    install(FILES ${GEN_BINARY_DIR}/liblzma.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)
endif()
