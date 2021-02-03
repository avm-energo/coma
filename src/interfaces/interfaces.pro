include( ../common.pri )
include( ../lib.pri )

QT += core concurrent serialport network
QT -= gui widgets

TARGET = interfaces$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += INTERFACES_LIBRARY

INCLUDEPATH += ./../../include/hidapi

SOURCES += \
    baseinterface.cpp \
    iec104.cpp \
    iec104thread.cpp \
    interfacesettings.cpp \
    modbus.cpp \
    modbusprivate.cpp \
    modbusthread.cpp \
    protocom.cpp \
    protocom_p.cpp \
    protocomthread.cpp \
    serialport.cpp \
    usbhidport.cpp \
    usbhidportinfo.cpp \
    usbhidsettings.cpp

HEADERS += \
    baseinterface.h \
    iec104.h \
    iec104private.h \
    iec104thread.h \
    interfacesettings.h \
    modbus.h \
    modbusprivate.h \
    modbusthread.h \
    protocom.h \
    protocom_p.h \
    protocomprivate.h \
    protocomthread.h \
    serialport.h \
    settingstypes.h \
    usbhidport.h \
    usbhidportinfo.h \
    usbhidsettings.h
