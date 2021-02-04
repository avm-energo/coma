include( ../common.pri )

QT       += core gui printsupport network serialport widgets concurrent testlib xml
QT.testlib.CONFIG -= console
TARGET = AVM-Service
VERSION = 2.0.0

TEMPLATE = app

QMAKE_SUBSTITUTES += ../config.h.in

INCLUDEPATH += ../$${QXLSX_PARENTPATH}
INCLUDEPATH += ../$${QXLSX_HEADERPATH}
INCLUDEPATH += ../$${QXLSX_SOURCEPATH}

SOURCES += \
    coma.cpp \
    main.cpp \
    servicemodule.cpp
	
HEADERS += \
   coma.h \
    servicemodule.h

DESTDIR = $${BIN_PATH}/$${TARGET}
win32 {
    LIBS += -luser32
    contains(QMAKE_TARGET.arch, x86_64) {
       message("Windows x64 build")
       ## Windows x64 (64bit) specific build here
       CONFIG(debug, debug|release) {
       LIBS += -L$$PWD/../../lib/win64/debug/ -lhidapi
       } else {
       LIBS += -L$$PWD/../../lib/win64/release/ -lhidapi
       LIBS_FILES += \
       $$PWD/../../lib/win64/release/hidapi.dll
       }
    } else {
        message("Windows x86 build")
        ## Windows x86 (32bit) specific build here
        CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../../lib/win32/debug/ -lhidapi
        } else {
        LIBS += -L$$PWD/../../lib/win32/release/ -lhidapi
        LIBS_FILES += \
        $$PWD/../../lib/win32/release/hidapi.dll
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
    general

include(../deps.pri)

IMAGE_FILE += \
    $$PWD/../../images

ERROR_FILES += \
    $$PWD/../../errors

copyToDestDir($$IMAGE_FILE, $$DESTDIR/images/)
copyToDestDir($$ERROR_FILES, $$DESTDIR/errors/)
copyToDestDir($$LIBS_FILES, $$DESTDIR/)
