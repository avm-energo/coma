# Building a portable AppImage after `cmake --install`, using linuxdeploy + its Qt plugin.
# Requires linuxdeploy and linuxdeploy-plugin-qt to be reachable on PATH:
#   https://github.com/linuxdeploy/linuxdeploy/releases
#   https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases
#
# For the result to run on both an older Debian and a newer Fedora, this must be run
# on a build agent with the OLDEST glibc among the target systems.
#
# For a truly portable (not just distro-agnostic) AppImage, configure with
# -DBUILD_SHARED_LIBS=OFF so the in-house deps (libavm-widgets, libavm-qcustomplot,
# limereport, ...) get linked statically instead of being shipped as loose .so files.

find_program(LINUXDEPLOY_EXECUTABLE NAMES linuxdeploy linuxdeploy-x86_64.AppImage)
find_program(LINUXDEPLOY_PLUGIN_QT_EXECUTABLE NAMES linuxdeploy-plugin-qt linuxdeploy-plugin-qt-x86_64.AppImage)

# QT_QMAKE_EXECUTABLE isn't actually populated by this project's find_package(Qt6) call
# (verified: it stays unset) - fall back to the Qt6::qmake imported target it does provide.
if(NOT QT_QMAKE_EXECUTABLE AND TARGET Qt${QT_VERSION_MAJOR}::qmake)
  get_target_property(QT_QMAKE_EXECUTABLE Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
endif()
get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)

set(DESKTOP_FILE "${CMAKE_SOURCE_DIR}/package/src/${PROJECT_NAME}/ru.avmenergo.${PROJECT_NAME}.desktop")
set(ICON_FILE "${CMAKE_SOURCE_DIR}/images/icons/abatcher.png")

install(CODE "set(INSTALL_PREFIX \"${CMAKE_INSTALL_PREFIX}\")")
install(CODE "set(PROJECT_NAME \"${PROJECT_NAME}\")")
install(CODE "set(LINUXDEPLOY_EXECUTABLE \"${LINUXDEPLOY_EXECUTABLE}\")")
install(CODE "set(LINUXDEPLOY_PLUGIN_QT_EXECUTABLE \"${LINUXDEPLOY_PLUGIN_QT_EXECUTABLE}\")")
install(CODE "set(DESKTOP_FILE \"${DESKTOP_FILE}\")")
install(CODE "set(ICON_FILE \"${ICON_FILE}\")")
install(CODE "set(QT_BIN_DIR \"${QT_BIN_DIR}\")")

install(CODE [[
  if(NOT LINUXDEPLOY_EXECUTABLE)
    message(WARNING "linuxdeploy not found on PATH - skipping AppImage packaging. "
                     "Download from https://github.com/linuxdeploy/linuxdeploy/releases")
  elseif(NOT LINUXDEPLOY_PLUGIN_QT_EXECUTABLE)
    message(WARNING "linuxdeploy-plugin-qt not found on PATH - skipping AppImage packaging. "
                     "Download from https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases")
  elseif(NOT EXISTS "${DESKTOP_FILE}")
    message(WARNING "Desktop file not found: ${DESKTOP_FILE} - skipping AppImage packaging")
  else()
    set(APPDIR "${INSTALL_PREFIX}/AppDir")
    execute_process(
      COMMAND "${CMAKE_COMMAND}" -E env
        "QMAKE=${QT_BIN_DIR}/qmake"
        "LD_LIBRARY_PATH=${INSTALL_PREFIX}/lib"
        "${LINUXDEPLOY_EXECUTABLE}"
          --appdir "${APPDIR}"
          --executable "${INSTALL_PREFIX}/bin/${PROJECT_NAME}"
          --desktop-file "${DESKTOP_FILE}"
          --icon-file "${ICON_FILE}"
          --plugin qt
          --output appimage
      WORKING_DIRECTORY "${INSTALL_PREFIX}"
      RESULT_VARIABLE LINUXDEPLOY_RESULT)
    if(NOT LINUXDEPLOY_RESULT EQUAL 0)
      message(WARNING "linuxdeploy failed with exit code ${LINUXDEPLOY_RESULT}")
    else()
      message(STATUS "AppImage built in ${INSTALL_PREFIX}")
    endif()
  endif()
]])
