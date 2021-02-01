include( ../common.pri )

QT       += core gui printsupport network serialport widgets concurrent testlib xml
QT.testlib.CONFIG -= console
TARGET = AVM-Service
VERSION = 2.0.0

TEMPLATE = app

QMAKE_SUBSTITUTES += ../config.h.in

QXLSX_PARENTPATH=./../../include/QXlsx/QXlsx         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./../../include/QXlsx/QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./../../include/QXlsx/QXlsx/source/  # current QXlsx source path is ./source/
include(./../../include/QXlsx/QXlsx/QXlsx.pri)

SOURCES += \
    coma.cpp \
    main.cpp \
    servicemodule.cpp
	
HEADERS += \
    coma.h \
    servicemodule.h

win32 {
    LIBS += -luser32
    contains(QMAKE_TARGET.arch, x86_64) {
       message("Windows x64 build")
       ## Windows x64 (64bit) specific build here
       CONFIG(debug, debug|release) {
       LIBS += -L$$PWD/../../lib/win64/debug/ -lhidapi
       DESTDIR = $${PWD}/../../bin/$${TARGET}/win64/debug
       } else {
       LIBS += -L$$PWD/../../lib/win64/release/ -lhidapi
       DESTDIR = $${PWD}/../../bin/$${TARGET}/win64/release
       LIBS_FILES += \
       $$PWD/../../lib/win64/release/hidapi.dll
       }
    } else {
        message("Windows x86 build")
        ## Windows x86 (32bit) specific build here
        CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../../lib/win32/debug/ -lhidapi
        DESTDIR = $${PWD}/../../bin/$${TARGET}/win32/debug
        } else {
        LIBS += -L$$PWD/../../lib/win32/release/ -lhidapi
        DESTDIR = $${PWD}/../../bin/$${TARGET}/win32/release
        LIBS_FILES += \
        $$PWD/../../lib/win32/release/hidapi.dll
        }
    }
}

LIBS +=  \
    -lCheck$${LIB_SUFFIX} \
    -lConfig$${LIB_SUFFIX} \
    -lDatablock$${LIB_SUFFIX} \
    -lDialogs$${LIB_SUFFIX} \
    -lInterfaces$${LIB_SUFFIX} \
    -lModels$${LIB_SUFFIX} \
    -lModule$${LIB_SUFFIX} \
    -lStartup$${LIB_SUFFIX} \
    -lWidgets$${LIB_SUFFIX} \
    -lGeneral$${LIB_SUFFIX}
