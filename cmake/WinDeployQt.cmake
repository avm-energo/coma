# function for

function(win_deploy_qt PROJECT_NAME QT_VERSION_MAJOR QT_BIN_DIR)
    # Set installation-time CMake variables
    message(STATUS "windeployqt path: " ${QT_BIN_DIR}/windeployqt.exe)
    install(CODE "set(QT_BIN_DIR \"${QT_BIN_DIR}\")")
    install(CODE "set(QT_VERSION_MAJOR \"${QT_VERSION_MAJOR}\")")
    install(CODE "set(PROJECT_NAME \"${PROJECT_NAME}\")")

    # Install Qt dependencies for Coma and LimeReport
    install(
      CODE [[
        execute_process(COMMAND ${QT_BIN_DIR}/windeployqt.exe --no-quick-import --no-system-d3d-compiler
            --no-virtualkeyboard --no-compiler-runtime --no-webkit2 --no-angle --no-opengl-sw
            ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH}/${PROJECT_NAME}.exe
            ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH}/limereport-qt${QT_VERSION_MAJOR}.dll
          WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${INSTALL_PROJECT_PATH})
      ]])
endfunction()
