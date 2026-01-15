# Setting installation project path for project deploying

install(CODE "set(INSTALL_PREFIX \"${CMAKE_INSTALL_PREFIX}\")")
install(CODE "set(EXECUTABLES \"$<TARGET_FILE:${PROJECT_NAME}>\")")

# Using windeployqt for installation Qt dependencies
get_filename_component(QT_BIN_DIR ${QT_QMAKE_EXECUTABLE} DIRECTORY)
set(DEPENDENCY_PATHS "${CMAKE_INSTALL_PREFIX}/bin" "${QT_BIN_DIR}")

# Transfer the values into the install script
install(CODE "set(DEPENDENCY_PATHS \"${DEPENDENCY_PATHS}\")")
install(CODE "set(QT_BIN_DIR \"${QT_BIN_DIR}\")")
install(CODE "set(PROJECT_NAME \"${PROJECT_NAME}\")")

LIST(APPEND pre_exclude_regexes "api-ms-.*") # windows API
LIST(APPEND pre_exclude_regexes "ext-ms-.*") # windows API
LIST(APPEND pre_exclude_regexes "ieshims.dll") # windows API
LIST(APPEND pre_exclude_regexes "emclient.dll") # windows API
LIST(APPEND pre_exclude_regexes "devicelockhelpers.dll") # windows API

LIST(APPEND post_exclude_regexes ".*system32/.*dll") # windows system dlls
install(CODE "set(pre_exclude_regexes \"${pre_exclude_regexes}\")")
install(CODE "set(post_exclude_regexes \"${post_exclude_regexes}\")")

# Runtime dependencies installation
install(CODE [[
  message("DEP_PATHS: ${DEPENDENCY_PATHS}")
  file(GET_RUNTIME_DEPENDENCIES
    DIRECTORIES ${DEPENDENCY_PATHS}
    RESOLVED_DEPENDENCIES_VAR RES
    UNRESOLVED_DEPENDENCIES_VAR UNRES
    PRE_EXCLUDE_REGEXES ${pre_exclude_regexes}
    POST_EXCLUDE_REGEXES ${post_exclude_regexes}
    CONFLICTING_DEPENDENCIES_PREFIX CONFLICTING_DEPENDENCIES
    EXECUTABLES ${EXECUTABLES})

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
      DESTINATION "${INSTALL_PREFIX}/bin"
      TYPE SHARED_LIBRARY
      FOLLOW_SYMLINK_CHAIN
      FILES "${_file}")
  endforeach()
  execute_process(COMMAND ${QT_BIN_DIR}/windeployqt.exe --no-quick-import --no-system-d3d-compiler
      --no-compiler-runtime --no-opengl-sw
      --skip-plugin-types generic,iconengines,networkinformation,styles,tls,translations
      ${INSTALL_PREFIX}/bin/${PROJECT_NAME}.exe)
]])
