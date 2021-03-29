include( ../common.pri )

QT       += core gui printsupport network serialport widgets concurrent testlib xml
QT.testlib.CONFIG -= console
TARGET = AVM-Service
VERSION = 2.1.2

TEMPLATE = app

RC_ICONS = ../../coma.ico

QMAKE_SUBSTITUTES += config.h.in

INCLUDEPATH += ../$${QXLSX_PARENTPATH}
INCLUDEPATH += ../$${QXLSX_HEADERPATH}
INCLUDEPATH += ../$${QXLSX_SOURCEPATH}

include(AVM-Service.pri)

DESTDIR = $${BIN_PATH}/$${TARGET}
win32 {
    LIBS += -luser32
    contains(QMAKE_TARGET.arch, x86_64) {
       message("Windows x64 build")
       ## Windows x64 (64bit) specific build here
       CONFIG(debug, debug|release) {
       LIBS += -L$$PWD/../../lib/win64/$${BUILD_FLAG}/ -lhidapi
       } else {
       LIBS += -L$$PWD/../../lib/win64/$${BUILD_FLAG}/ -lhidapi
       LIBS_FILES += \
       $$PWD/../../lib/win64/$${BUILD_FLAG}/hidapi.dll
       }
    } else {
        message("Windows x86 build")
        ## Windows x86 (32bit) specific build here
        CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../../lib/win32/$${BUILD_FLAG}/ -lhidapi
        } else {
        LIBS += -L$$PWD/../../lib/win32/$${BUILD_FLAG}/ -lhidapi
        LIBS_FILES += \
        $$PWD/../../lib/win32/$${BUILD_FLAG}/hidapi.dll
        }
    }
}

unix {
LIBS += -lhidapi-hidraw
contains(QT_ARCH, x86_64) {
        message("Unix x64 build")
        ## Unix x64 (64bit) specific build here
        CONFIG(debug, debug|release) {

        }
    } else {
        message("Unix x86 build")
        ## Unix x86 (32bit) specific build here
        CONFIG(debug, debug|release) {

        }
    }
}


LIBRARIES += check \
    config \
    comaversion \
    datablock \
    dialogs \
    interfaces \
    models \
    module \
    startup \
    widgets \
    general \
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
