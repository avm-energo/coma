# Setting installation project path for project deploying
set(INSTALL_PROJECT_PATH ${PROJECT_TARGET_NAME}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME})
install(CODE "set(INSTALL_PROJECT_PATH \"${INSTALL_PROJECT_PATH}\")")

# Targets installation
install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH}
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH})

# Copying XML configuration
install(
  DIRECTORY ${CMAKE_SOURCE_DIR}/../settings
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH}
  FILES_MATCHING PATTERN "*.xml"
  PERMISSIONS OWNER_READ GROUP_READ)
# Copying lrxml report files
install(
  DIRECTORY ${CMAKE_SOURCE_DIR}/../reports
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH}
  FILES_MATCHING PATTERN "*.lrxml"
  PERMISSIONS OWNER_READ GROUP_READ)

# Runtime dependencies installation
install(CODE [[
  message("Comaresources: ${COMARES_BINDIR}")
  file(GET_RUNTIME_DEPENDENCIES
    PRE_EXCLUDE_REGEXES "api-ms-*"
    POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
    DIRECTORIES
      "$<TARGET_PROPERTY:AVM-Service,LINK_DIRECTORIES>"
      "${CMAKE_INSTALL_PREFIX}/bin"
      "${hidapi_BIN_DIRS}"
      "${COMARES_BINDIR}"
      "${COMARES_BINDIR}/Release"
    RESOLVED_DEPENDENCIES_VAR RES
    UNRESOLVED_DEPENDENCIES_VAR UNRES
    CONFLICTING_DEPENDENCIES_PREFIX CONFLICTING_DEPENDENCIES
    EXECUTABLES $<TARGET_FILE:AVM-Service>)

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
      FILES "${_file}")
  endforeach()
]])

# Using windeployqt for installation Qt dependencies
get_filename_component(QT_BIN_DIR ${QT_QMAKE_EXECUTABLE} DIRECTORY)
win_deploy_qt(${PROJECT_NAME} ${QT_VERSION_MAJOR} ${QT_BIN_DIR})
