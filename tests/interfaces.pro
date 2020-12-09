QT -= gui
QT += network serialport concurrent
CONFIG += c++17 console
CONFIG -= app_bundle


QMAKE_TARGET_COMPANY = AVM-Energo
QMAKE_TARGET_COPYRIGHT = AVM-Energo
QMAKE_TARGET_PRODUCT = AVTUK-S

TARGET = Interfaces
DEFINES += PROGNAME='\\"AVM-Service\\"'
DEFINES += DEVICETYPE=1 # 1 - module, 2 - pribor, for diagnostic messages
DEFINES += SOFTDEVELOPER='\\"$$QMAKE_TARGET_COMPANY\\"'
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
DEFINES += GIT_VERSION='$(shell git describe --always)'
DEFINES += COMAVERSION='\\"$$VERSION\\"'
VERSION = $$GIT_VERSION
DEFINES += BUILD_DATE='"\\\"$(shell date)\\\""'

SOURCES += \
    ../gen/board.cpp \
    ../gen/datamanager.cpp \
    ../gen/error.cpp \
    ../gen/files.cpp \
    ../gen/helper.cpp \
    ../gen/logclass.cpp \
    ../gen/logger.cpp \
    ../gen/pch.cpp \
    ../gen/s2.cpp \
    ../gen/stdfunc.cpp \
    ../interfaces/baseinterface.cpp \
    ../interfaces/ethernet.cpp \
    ../interfaces/iec104.cpp \
    ../interfaces/iec104thread.cpp \
    ../interfaces/modbus.cpp \
    ../interfaces/modbusthread.cpp \
    ../interfaces/protocom.cpp \
    ../interfaces/protocomthread.cpp \
    ../interfaces/serialport.cpp \
    ../interfaces/usbhidport.cpp \
    ../interfaces/usbhidportinfo.cpp \
    console.cpp \
    interfaces.cpp

PRECOMPILED_HEADER = ../gen/pch.h

HEADERS += \
    ../gen/board.h \
    ../gen/datamanager.h \
    ../gen/datatypes.h \
    ../gen/error.h \
    ../gen/files.h \
    ../gen/helper.h \
    ../gen/logclass.h \
    ../gen/logger.h \
    ../gen/s2.h \
    ../gen/singleton.h \
    ../gen/stdfunc.h \
    ../interfaces/baseinterface.h \
    ../interfaces/ethernet.h \
    ../interfaces/iec104.h \
    ../interfaces/iec104private.h \
    ../interfaces/iec104thread.h \
    ../interfaces/modbus.h \
    ../interfaces/modbusprivate.h \
    ../interfaces/modbusthread.h \
    ../interfaces/protocom.h \
    ../interfaces/protocomprivate.h \
    ../interfaces/protocomthread.h \
    ../interfaces/serialport.h \
    ../interfaces/settingstypes.h \
    ../interfaces/usbhidport.h \
    ../interfaces/usbhidportinfo.h \
    console.h

INCLUDEPATH += $$PWD/../../includes


win32 {
    LIBS += -luser32
    contains(QMAKE_TARGET.arch, x86_64) {
       message("Windows x64 build")
       ## Windows x64 (64bit) specific build here
       CONFIG(debug, debug|release) {
       LIBS += -L$$PWD/../../libs/win64/debug/ -lhidapi
       DESTDIR = $${PWD}/../../build/win64/debug
       } else {
       LIBS += -L$$PWD/../../libs/win64/release/ -lhidapi
       DESTDIR = $${PWD}/../../build/win64/release
       LIBS_FILES += \
       $$PWD/../../libs/win64/release/hidapi.dll
       }
    } else {
        message("Windows x86 build")
        ## Windows x86 (32bit) specific build here
        CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../../libs/win32/debug/ -lhidapi
        DESTDIR = $${PWD}/../../build/win32/debug
        } else {
        LIBS += -L$$PWD/../../libs/win32/release/ -lhidapi
        DESTDIR = $${PWD}/../../build/win32/release
        LIBS_FILES += \
        $$PWD/../../libs/win32/release/hidapi.dll
        }
    }
}

unix {
LIBS += -lhidapi-hidraw
contains(QT_ARCH, x86_64) {
        message("Unix x64 build")
        ## Unix x64 (64bit) specific build here
        CONFIG(debug, debug|release) {
        DESTDIR = $${PWD}/../../build/linux64/debug
        } else {
        DESTDIR = $${PWD}/../../build/linux64/release
        }
    } else {
        message("Unix x86 build")
        ## Unix x86 (32bit) specific build here
        CONFIG(debug, debug|release) {
        DESTDIR = $${PWD}/../../build/linux32/debug
        } else {
        DESTDIR = $${PWD}/../../build/linux32/release
        }
    }
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
