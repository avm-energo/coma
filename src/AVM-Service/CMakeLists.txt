project(AVM-Service VERSION 2.5.1)

find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Widgets REQUIRED)

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

target_link_libraries(${PROJECT_NAME} PUBLIC
  s2
  interfaces
  comaresources
  module
  widgets
  dialogs
  models
  startup
  datablocks
  comaversion
  oscillograms
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Widgets)

# Copy settings one time at first build
set(SVC_CHECK_PATH "${CMAKE_CURRENT_BINARY_DIR}/check.txt")
if(NOT EXISTS ${SVC_CHECK_PATH})
  file(TOUCH ${SVC_CHECK_PATH})
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
  include(WinDeployService)
elseif(CMAKE_SYSTEM_NAME_LOWER STREQUAL "linux")
  message(STATUS "Deploy ${PROJECT_NAME} for Linux")
  set(COMPONENT_NAME "AVMSERVICE")
  include(ComponentDeploy)
endif()
