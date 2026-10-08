# Setting installation project path for project deploying

install(CODE "set(INSTALL_PREFIX \"${CMAKE_INSTALL_PREFIX}\")")
install(CODE "set(EXECUTABLES \"$<TARGET_FILE:${PROJECT_NAME}>\")")

# Using windeployqt for installation Qt dependencies
# QT_QMAKE_EXECUTABLE isn't set by Qt6Config.cmake itself (confirmed on Linux; the CMake
# config files are the same cross-platform) - it only ends up defined when Qt Creator injects
# it via its Kit setup. On a plain command-line configure (e.g. a CI agent) fall back to the
# Qt6::qmake imported target it does provide.
if(NOT QT_QMAKE_EXECUTABLE AND TARGET Qt${QT_VERSION_MAJOR}::qmake)
  get_target_property(QT_QMAKE_EXECUTABLE Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
endif()
get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
set(DEPENDENCY_PATHS "${CMAKE_INSTALL_PREFIX}/bin" "${libavm-gen_BINARY_DIR}" "${libavm-widgets_BINARY_DIR}" "${QT_BIN_DIR}")

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

# App-local deployment of the MSVC C/C++ runtime (vcruntime140.dll, msvcp140.dll, ...)
# so the portable build doesn't depend on vc_redist being installed on the target machine.
# See: https://learn.microsoft.com/cpp/windows/deploying-native-desktop-applications-visual-cpp
if(DEFINED ENV{VCToolsRedistDir})
  # VCToolsRedistDir already includes the toolchain version, e.g. .../VC/Redist/MSVC/14.38.33130/
  # the CRT folder underneath is named after the toolset (VC143 for VS2022) which is a
  # different number, so glob for it instead of trying to compute the name.
  file(GLOB VC_REDIST_CRT_DIRS "$ENV{VCToolsRedistDir}x64/Microsoft.VC*.CRT")
  if(VC_REDIST_CRT_DIRS)
    list(GET VC_REDIST_CRT_DIRS 0 VC_REDIST_DIR)
  endif()
endif()
if(NOT VC_REDIST_DIR OR NOT EXISTS "${VC_REDIST_DIR}")
  # Fallback: matches the path already hardcoded in iss/avm-debug.iss for this build agent
  set(VC_REDIST_DIR "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Redist/MSVC/v143/x64/Microsoft.VC143.CRT")
endif()

install(CODE "set(VC_REDIST_DIR \"${VC_REDIST_DIR}\")")
install(CODE [[
  if(EXISTS "${VC_REDIST_DIR}")
    file(GLOB VC_REDIST_DLLS "${VC_REDIST_DIR}/*.dll")
    message("VC redist app-local deployment from: ${VC_REDIST_DIR}")
    file(INSTALL DESTINATION "${INSTALL_PREFIX}/bin" TYPE FILE FILES ${VC_REDIST_DLLS})
  else()
    message(WARNING "VC_REDIST_DIR not found (${VC_REDIST_DIR}) - portable build will require vc_redist to be installed on target machine")
  endif()
]])
