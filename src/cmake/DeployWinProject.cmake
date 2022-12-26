# Setting installation project path for project deploying
set(INSTALL_PROJECT_PATH ${PROJECT_TARGET_NAME}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME})
message(STATUS "Install project path: ${INSTALL_PROJECT_PATH}")
install(CODE "set(INSTALL_PROJECT_PATH \"${INSTALL_PROJECT_PATH}\")")

# Getting OpenSSL binaries path
set(OpenSSL_BIN_DIRS "${OpenSSL_SSL_LIB_DIRS}/../bin")
message(STATUS "OpenSSL binaries directory: ${OpenSSL_BIN_DIRS}")
install(CODE "set(OpenSSL_BIN_DIR \"${OpenSSL_BIN_DIRS}\")")

# Getting windeployqt path
get_filename_component(QT_BIN_DIR ${QT_QMAKE_EXECUTABLE} DIRECTORY)
message(STATUS "windeployqt path: " ${QT_BIN_DIR}/windeployqt.exe)
install(CODE "set(QT_BIN_DIR \"${QT_BIN_DIR}\")")

# Install targets
install(
  TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH}
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH})

# Copying images
install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../copy_to_build/images
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH})

# Copying templates
install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../copy_to_build/templates
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH})

# Install runtime dependencies
install(CODE [[
    # Getting runtime dependencies for main project executable file
    file(GET_RUNTIME_DEPENDENCIES
      PRE_EXCLUDE_REGEXES "api-ms-*"
      POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
      DIRECTORIES
        "$<TARGET_PROPERTY:SUPiK,LINK_DIRECTORIES>"
        "$<TARGET_PROPERTY:QXlsx,IMPORTED_LOCATION>"
        "$<TARGET_PROPERTY:limereport,IMPORTED_LOCATION>"
      RESOLVED_DEPENDENCIES_VAR RESOLVED
        UNRESOLVED_DEPENDENCIES_VAR UNRESOLVED
        CONFLICTING_DEPENDENCIES_PREFIX CONFLICTING_DEPENDENCIES
        EXECUTABLES $<TARGET_FILE:SUPiK>)

    # Output resolved dependencies
    message("\n\nFound dependencies:")
    foreach(DEP ${RESOLVED})
      message("${DEP}")
    endforeach()
    # Output unresolved dependencies
    message("\n\nNot found dependencies:")
    foreach(DEP ${UNRESOLVED})
      message("${DEP}")
    endforeach()
    # Copying resolved dependencies to install project path
    foreach(_file ${RESOLVED})
      file(INSTALL
        DESTINATION "${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH}"
        TYPE SHARED_LIBRARY
        FILES "${_file}")
    endforeach()

    # Copying OpenSSL binaries to install project path
    file(GLOB_RECURSE openssl_libs ${OpenSSL_BIN_DIR}/*.dll)
    foreach(lib ${openssl_libs})
      file(INSTALL
        DESTINATION "${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH}"
        TYPE SHARED_LIBRARY
        FILES "${lib}")
    endforeach()

    # Copying Qt binaries to install project path with windeployqt
    execute_process(COMMAND ${QT_BIN_DIR}/windeployqt.exe --no-quick-import --no-system-d3d-compiler --no-virtualkeyboard
        --no-compiler-runtime --no-webkit2 --no-angle --no-opengl-sw ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH}/SUPiK.exe
      WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH})
    ]])
