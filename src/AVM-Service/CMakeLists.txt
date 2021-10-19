project(AVM-Service)

set(PROJECT_VERSION_MAJOR 2)
set(PROJECT_VERSION_MINOR 3)
set(PROJECT_VERSION_PATCH 3)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core Xml Widgets Network
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Xml Widgets Network
  REQUIRED)

configure_file(${CMAKE_SOURCE_DIR}/cconfig.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/versioninfo.rc.in
               ${CMAKE_CURRENT_BINARY_DIR}/versioninfo.rc @ONLY)

add_executable(
  ${PROJECT_NAME}
  main.cpp
  servicecoma.cpp
  servicecoma.h
  svcmodule.cpp
  svcmodule.h
  ../coma.rc
  ${CMAKE_CURRENT_BINARY_DIR}/versioninfo.rc)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
endif()

target_link_libraries(
  ${PROJECT_NAME}
  interfaces
  gen
  module
  check
  config
  widgets
  dialogs
  models
  startup
  comaversion
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Xml
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Network
  QXlsx)


if(CMAKE_SYSTEM_NAME_LOWER STREQUAL "windows")

  set(INSTALL_PROJECT_PATH ${PROJECT_TARGET_NAME}/${MODE_DIR}/${PROJECT_NAME})

  install(CODE "set(INSTALL_PROJECT_PATH \"${INSTALL_PROJECT_PATH}\")")

  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/../images
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/images)

  install(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH})

  install(
    DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>/images
    DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH}
    FILES_MATCHING
    PATTERN "*.png" PERMISSIONS OWNER_READ GROUP_READ)

  install(
    CODE [[
          file(GET_RUNTIME_DEPENDENCIES
              PRE_EXCLUDE_REGEXES
                "api-ms-*"
              POST_EXCLUDE_REGEXES
                ".*system32/.*\\.dll"
              DIRECTORIES
                "$<TARGET_PROPERTY:AVM-Service,LINK_DIRECTORIES>"
                "$<TARGET_PROPERTY:QXlsx,IMPORTED_LOCATION>"
                ${hidapi_BIN_DIRS}
              RESOLVED_DEPENDENCIES_VAR RES
              UNRESOLVED_DEPENDENCIES_VAR UNRES
              CONFLICTING_DEPENDENCIES_PREFIX CONFLICTING_DEPENDENCIES
              EXECUTABLES $<TARGET_FILE:AVM-Service>
          )

          message("\n\nFound dependencies :")
          foreach(DEP ${RES})
              message("${DEP}")
          endforeach()
          message("\n\nNot found dependencies :")
          foreach(DEP ${UNRES})
              message("${DEP}")
          endforeach()
          foreach(_file ${RES})
              file(INSTALL
                  DESTINATION "${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH}"
                  TYPE SHARED_LIBRARY
                  #TODO requires 3.15+
                  #FOLLLOW_SYMLINK_CHAIN
                  FILES "${_file}"
              )
          endforeach()
      ]])

  get_filename_component(QT_BIN_DIR ${QT_QMAKE_EXECUTABLE} DIRECTORY)

  message(STATUS "Qt binaries directory: " ${QT_BIN_DIR})

  install(CODE "set(QT_BIN_DIR \"${QT_BIN_DIR}\")")

  message(STATUS "windeployqt: " ${QT_BIN_DIR}/windeployqt.exe)

  install(
    CODE [[execute_process(COMMAND ${QT_BIN_DIR}/windeployqt.exe --no-quick-import --no-system-d3d-compiler --no-virtualkeyboard --no-compiler-runtime --no-webkit2 --no-angle --no-opengl-sw ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH}/AVM-Service.exe
        WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH})]])

endif()
