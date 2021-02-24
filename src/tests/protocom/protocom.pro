include( ../../common.pri )
#QT -= gui
QT += gui widgets network serialport concurrent xml printsupport
CONFIG += c++17 console

TARGET = Interface

TEMPLATE = app

SOURCES += \
    $$PWD/console.cpp \
    $$PWD/main.cpp
HEADERS += \
    $$PWD/console.h



DESTDIR = $${BIN_PATH}/$${TARGET}
win32 {
    LIBS += -luser32
    contains(QMAKE_TARGET.arch, x86_64) {
       message("Windows x64 build")
       ## Windows x64 (64bit) specific build here
       CONFIG(debug, debug|release) {
       LIBS += -L$$PWD/../../../lib/win64/$${BUILD_FLAG}/ -lhidapi
       } else {
       LIBS += -L$$PWD/../../../lib/win64/$${BUILD_FLAG}/ -lhidapi
       LIBS_FILES += \
       $$PWD/../../../lib/win64/$${BUILD_FLAG}/hidapi.dll
       }
    } else {
        message("Windows x86 build")
        ## Windows x86 (32bit) specific build here
        CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../../../lib/win32/$${BUILD_FLAG}/ -lhidapi
        } else {
        LIBS += -L$$PWD/../../../lib/win32/$${BUILD_FLAG}/ -lhidapi
        LIBS_FILES += \
        $$PWD/../../../lib/win32/$${BUILD_FLAG}/hidapi.dll
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

include(../../deps.pri)



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
