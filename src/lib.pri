DESTDIR = $${LIBS_PATH}/
win32: DLLDESTDIR = $${BIN_PATH}/
VERSION = 1.0.0
CONFIG += staticlib
# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
