project(AVM-Debug)

set(PROJECT_VERSION_MAJOR 2)
set(PROJECT_VERSION_MINOR 5)
set(PROJECT_VERSION_PATCH 1)

find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Widgets REQUIRED)

configure_file(${CMAKE_SOURCE_DIR}/cconfig.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

configure_file(${CMAKE_SOURCE_DIR}/versioninfo.rc.in
               ${CMAKE_CURRENT_BINARY_DIR}/versioninfo.rc @ONLY)

file(GLOB_RECURSE files_cpp ${PROJECT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE files_h ${PROJECT_SOURCE_DIR}/*.h)

add_executable(${PROJECT_NAME}
  ${files_cpp} ${files_h} ../coma.rc
  ${CMAKE_CURRENT_BINARY_DIR}/versioninfo.rc)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC
  interfaces
  s2
  module
  widgets
  dialogs
  models
  startup
  tune
  oscillograms
  datablocks
  comaversion
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Widgets)

if(ENABLE_EMULATOR)
  target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_EMULATOR)
endif()

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
  set(COMPONENT_NAME "AVMDebug")
  set(DEPLOY_IN  "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/LinuxDeploy.cmake.in")
  set(DEPLOY_OUT "${CMAKE_CURRENT_BINARY_DIR}/LinuxDeployDebug.cmake")
  configure_file(${DEPLOY_IN} ${DEPLOY_OUT} @ONLY)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}")
  include(LinuxDeployDebug)

#  install(
#    TARGETS ${PROJECT_NAME} gen QXlsx limereport-qt${QT_VERSION_MAJOR} qcustomplot
#    RUNTIME DESTINATION bin COMPONENT ${COMPONENT_NAME}
#    LIBRARY DESTINATION lib COMPONENT ${COMPONENT_NAME}
#    ARCHIVE DESTINATION lib COMPONENT ${COMPONENT_NAME})
#  install(
#    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../settings"
#    DESTINATION share/coma
#    COMPONENT ${COMPONENT_NAME})
endif()
