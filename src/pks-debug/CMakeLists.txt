project(pks-debug VERSION ${CMAKE_PROJECT_VERSION})

option(AVM_TEST "Debug version of the program" OFF)

add_compile_definitions(PROGSIZE=4)

if(AVM_TEST)
  add_compile_definitions(AVM_DEBUG)
endif()

# Getting appdata directory path
if(SYSTEM_NAME STREQUAL "windows")
    # On windows
    set(APPDATA_STR "$ENV{APPDATA}/AVM-Energo/${PROJECT_NAME}")
    string(REPLACE "\\" "/" APPDATA ${APPDATA_STR})
else()
    # On linux
    set(APPDATA $ENV{HOME}/.config/AVM-Energo/${PROJECT_NAME})
endif()

configure_file(
  ${CMAKE_SOURCE_DIR}/cconfig.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h
@ONLY)

configure_file(
  ${CMAKE_SOURCE_DIR}/versioninfo.rc.in
  ${CMAKE_CURRENT_BINARY_DIR}/versioninfo.rc
@ONLY)

configure_file(
  ${CMAKE_SOURCE_DIR}/avm.tsv.in
  ${CMAKE_SOURCE_DIR}/package/src/${PROJECT_NAME}.tsv
@ONLY)

file(GLOB_RECURSE files_cpp ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE files_h ${PROJECT_SOURCE_DIR}/*.h)

add_executable(${PROJECT_NAME}
  ${files_cpp} ${files_h} ../coma.rc ../../images/images.qrc
  ${CMAKE_CURRENT_BINARY_DIR}/versioninfo.rc)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

# tell windows to start prog without console
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC coma)

if(ENABLE_EMULATOR)
  target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_EMULATOR)
endif()

# Getting appdata directory path
if(SYSTEM_NAME STREQUAL "windows")
    # On windows
    set(APPDATA_STR "$ENV{APPDATA}/AVM-Energo/${PROJECT_NAME}")
    string(REPLACE "\\" "/" APPDATA ${APPDATA_STR})
    message(STATUS "Set appdata to ${APPDATA}")
else()
    # On linux
    set(APPDATA $ENV{HOME}/.config/AVM-Energo/${PROJECT_NAME})
    message(STATUS "Set appdata to ${APPDATA}")
endif()

# Copy settings one time at first build
set(DBG_CHECK_PATH "${CMAKE_BINARY_DIR}/pkdn-check.txt")
if(NOT EXISTS ${DBG_CHECK_PATH})
  file(TOUCH ${DBG_CHECK_PATH})
  # Copying reports lrxml files to appdata directory after building
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/../../reports"
    "${APPDATA}/reports"
    COMMENT "Copying reports...")
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/../../images"
    "${APPDATA}/images"
    COMMENT "Copying images...")
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/../../errors/ermsgs.dat"
    "${APPDATA}"
    COMMENT "Copying error messages...")
endif()

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Cross-platform deploy target
if(SYSTEM_NAME STREQUAL "windows")
  include(windeploy)
elseif(SYSTEM_NAME STREQUAL "linux")
  set(COMPONENT_NAME "PKSDEBUG")
#  include(LinuxDeploy)
endif()
