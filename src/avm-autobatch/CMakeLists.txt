project(avm-autobatcher VERSION 0.0.1)

# Find required Qt components
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS REQUIRED)

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
  ${files_cpp} ${files_h} ../../images/images.qrc
  ../abatcher/resources/firmware.qrc
  ../../settings/settings.qrc
  ${CMAKE_CURRENT_BINARY_DIR}/versioninfo.rc)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${CMAKE_PROJECT_VERSION})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
endif()

# Link against coma, abatcher library, and Qt Widgets
target_link_libraries(${PROJECT_NAME} PUBLIC
  abatcher
)

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
  set(COMPONENT_NAME "AVMABATCHER")
  include(linuxdeploy)
endif()