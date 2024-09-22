project(AVM-Debug VERSION ${CMAKE_PROJECT_VERSION})


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
if(SYSTEM_NAME STREQUAL "windows")
    # On windows
    set(APPDATA_STR "$ENV{LOCALAPPDATA}/${PROJECT_NAME}")
    string(REPLACE "\\" "/" APPDATA ${APPDATA_STR})
else()
    # On linux
    set(APPDATA $ENV{HOME}/.local/share/${PROJECT_NAME})
endif()

# Copy settings one time at first build
set(DBG_CHECK_PATH "${CMAKE_CURRENT_BINARY_DIR}/check.txt")
if(NOT EXISTS ${DBG_CHECK_PATH})
  file(TOUCH ${DBG_CHECK_PATH})
  # Copying XML settings to appdata directory after building
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/../../settings"
    ${APPDATA} COMMENT "Copying XML configs...")
  # Copying reports lrxml files to appdata directory after building
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/../../reports"
    "${APPDATA}/reports"
    COMMENT "Copying reports...")
endif()

install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Copying XML configuration
install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../settings"
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  FILES_MATCHING PATTERN "*.xml"
  PERMISSIONS OWNER_READ GROUP_READ)
# Copying lrxml report files
install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../reports"
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  FILES_MATCHING PATTERN "*.lrxml"
  PERMISSIONS OWNER_READ GROUP_READ)

# Cross-platform deploy target
if(SYSTEM_NAME STREQUAL "windows")
  include(WinDeploy)
elseif(SYSTEM_NAME STREQUAL "linux")
  set(COMPONENT_NAME "AVMDEBUG")
  include(LinuxDeploy)
endif()
