project(AVM-Debug VERSION 2.8.0)

configure_file(
  ${CMAKE_SOURCE_DIR}/cconfig.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h
@ONLY)

configure_file(
  ${CMAKE_SOURCE_DIR}/versioninfo.rc.in
  ${CMAKE_CURRENT_BINARY_DIR}/versioninfo.rc
@ONLY)

file(GLOB_RECURSE files_cpp ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE files_h ${PROJECT_SOURCE_DIR}/*.h)

add_executable(${PROJECT_NAME}
  ${files_cpp} ${files_h} ../coma.rc
  ${CMAKE_CURRENT_BINARY_DIR}/versioninfo.rc)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC coma_core)

if(ENABLE_EMULATOR)
  target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_EMULATOR)
endif()

# Getting appdata directory path
if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
    # On windows
    set(APPDATA_STR "$ENV{LOCALAPPDATA}/${PROJECT_NAME}")
    string(REPLACE "\\" "/" APPDATA ${APPDATA_STR})
else()
    # On linux
    set(APPDATA $ENV{HOME}/.local/share/${PROJECT_NAME})
endif()
message(STATUS "Local appdata path: ${APPDATA}")

# Copy settings one time at first build
set(DBG_CHECK_PATH "${CMAKE_CURRENT_BINARY_DIR}/check.txt")
if(NOT EXISTS ${DBG_CHECK_PATH})
  file(TOUCH ${DBG_CHECK_PATH})
  # Copying XML settings to appdata directory after building
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/../../settings
    ${APPDATA})
endif()

# Cross-platform deploy target
if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")
  message(STATUS "Deploy ${PROJECT_NAME} for Windows")
  include(WinDeployDebug)
elseif(CMAKE_SYSTEM_NAME_LOWER STREQUAL "linux")
  message(STATUS "Deploy ${PROJECT_NAME} for Linux")
  set(COMPONENT_NAME "AVMDEBUG")
  include(ComponentDeploy)
endif()
