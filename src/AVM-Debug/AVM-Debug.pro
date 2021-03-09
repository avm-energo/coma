include( ../common.pri )

QT       += core gui printsupport network serialport widgets concurrent testlib xml qml svg
QT.testlib.CONFIG -= console
TARGET = AVM-Debug
VERSION = 2.1.1
GIT_HASH = $$system(git rev-parse --short=8 HEAD)
TEMPLATE = app

RC_ICONS = ../../coma.ico

QMAKE_SUBSTITUTES += config.h.in

INCLUDEPATH += ../$${QXLSX_PARENTPATH}
INCLUDEPATH += ../$${QXLSX_HEADERPATH}
INCLUDEPATH += ../$${QXLSX_SOURCEPATH}
INCLUDEPATH += ../$${LIME_HEADERPATH}

include(AVM-Debug.pri)

DESTDIR = $${BIN_PATH}/$${TARGET}
win32 {
    LIBS += -luser32
    contains(QMAKE_TARGET.arch, x86_64) {
       message("Windows x64 build")
       ## Windows x64 (64bit) specific build here
       CONFIG(debug, debug|release) {
       LIBS += -L$$PWD/../../lib/win64/debug/ -llimereport$${LIB_SUFFIX} -lhidapi
       } else {
       LIBS += -L$$PWD/../../lib/win64/$${BUILD_FLAG}/ -llimereport$${LIB_SUFFIX} -lhidapi
       LIBS_FILES += \
       $$PWD/../../lib/win64/$${BUILD_FLAG}/hidapi.dll \
       $$PWD/../../lib/win64/$${BUILD_FLAG}/limereport.dll \
       $$PWD/../../lib/win64/$${BUILD_FLAG}/QtZint.dll
       }
    } else {
        message("Windows x86 build")
        ## Windows x86 (32bit) specific build here
        CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../../lib/win32/$${BUILD_FLAG}/ -llimereport$${LIB_SUFFIX} -lhidapi
        } else {
        LIBS += -L$$PWD/../../lib/win32/$${BUILD_FLAG}/ -llimereport$${LIB_SUFFIX} -lhidapi
        LIBS_FILES += \
        $$PWD/../../lib/win32/$${BUILD_FLAG}/hidapi.dll \
        $$PWD/../../lib/win32/$${BUILD_FLAG}/limereport.dll \
        $$PWD/../../lib/win32/$${BUILD_FLAG}/QtZint.dll
        }
    }
}

unix {
LIBS += -lhidapi-hidraw
contains(QT_ARCH, x86_64) {

        message("Unix x64 build")
        LIBS += -L$$PWD/../../lib/unix64/$${BUILD_FLAG} -llimereport$${LIB_SUFFIX} -lQtZint$${LIB_SUFFIX}
        ## Unix x64 (64bit) specific build here
        CONFIG(debug, debug|release) {

        }
    } else {
        message("Unix x86 build")
        LIBS += -L$$PWD/../../lib/unix32/$${BUILD_FLAG} -llimereport$${LIB_SUFFIX} -lQtZint$${LIB_SUFFIX}
        ## Unix x86 (32bit) specific build here
        CONFIG(debug, debug|release) {

        }
    }
}


LIBRARIES += check \
    config \
    datablock \
    dialogs \
    interfaces \
    models \
    module \
    startup \
    widgets \
    general \
    tune \
    avtuk \
    qcustomplot

include(../deps.pri)

IMAGE_FILE += \
    $$PWD/../../images

ERROR_FILES += \
    $$PWD/../../errors



copyToDestDir($$IMAGE_FILE, $$DESTDIR/images/)
copyToDestDir($$ERROR_FILES, $$DESTDIR/errors/)
copyToDestDir($$LIBS_FILES, $$DESTDIR/)

