project(avm-service VERSION ${CMAKE_PROJECT_VERSION})

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
  ${files_cpp} ${files_h} ../coma.rc ../../images/images.qrc
  ${CMAKE_CURRENT_BINARY_DIR}/versioninfo.rc)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC coma)

# Getting appdata directory path
if(SYSTEM_NAME STREQUAL "windows")
    # On windows
    set(APPDATA_STR "$ENV{APPDATA}/AVM-Energo/${PROJECT_NAME}")
    string(REPLACE "\\" "/" APPDATA ${APPDATA_STR})
else()
    # On linux
    set(APPDATA $ENV{HOME}/.config/AVM-Energo/${PROJECT_NAME})
endif()

# Copy settings one time at first build
set(SVC_CHECK_PATH "${CMAKE_BINARY_DIR}/check.txt")
if(NOT EXISTS ${SVC_CHECK_PATH})
  file(TOUCH ${SVC_CHECK_PATH})
  # Copying XML settings to appdata directory after building
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/../../settings"
    ${APPDATA})
  # Copying reports lrxml files to appdata directory after building
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/../../reports"
    "${APPDATA}/reports")
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
  set(COMPONENT_NAME "AVMSERVICE")
endif()
