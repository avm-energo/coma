include( ../common.pri )

QT       += core gui printsupport network serialport widgets concurrent testlib xml
QT.testlib.CONFIG -= console
TARGET = AVM-Debug
VERSION = 2.0.0

TEMPLATE = app

QMAKE_SUBSTITUTES += ../config.h.in

SOURCES += \
    coma.cpp \
    main.cpp \
    tunemodule.cpp
	
HEADERS += \
    coma.h \
    tunemodule.h

win32 {
    LIBS += -luser32
    contains(QMAKE_TARGET.arch, x86_64) {
       message("Windows x64 build")
       ## Windows x64 (64bit) specific build here
       CONFIG(debug, debug|release) {
       LIBS += -L$$PWD/../../lib/win64/debug/ -llimereportd -lhidapi
       DESTDIR = $${PWD}/../../bin/$${TARGET}/win64/debug
       } else {
       LIBS += -L$$PWD/../../lib/win64/release/ -llimereport -lhidapi
       DESTDIR = $${PWD}/../../bin/$${TARGET}/win64/release
       LIBS_FILES += \
       $$PWD/../../lib/win64/release/hidapi.dll \
       $$PWD/../../lib/win64/release/limereport.dll \
       $$PWD/../../lib/win64/release/QtZint.dll
       }
    } else {
        message("Windows x86 build")
        ## Windows x86 (32bit) specific build here
        CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../../lib/win32/debug/ -llimereportd -lhidapi
        DESTDIR = $${PWD}/../../bin/$${TARGET}/win32/debug
        } else {
        LIBS += -L$$PWD/../../lib/win32/release/ -llimereport -lhidapi
        DESTDIR = $${PWD}/../../bin/$${TARGET}/win32/release
        LIBS_FILES += \
        $$PWD/../../lib/win32/release/hidapi.dll \
        $$PWD/../../lib/win32/release/limereport.dll \
        $$PWD/../../lib/win32/release/QtZint.dll
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
    -lGeneral$${LIB_SUFFIX} \
    -lTune$${LIB_SUFFIX} \
