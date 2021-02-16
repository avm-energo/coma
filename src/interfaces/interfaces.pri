
CONFIG += c++17
CONFIG -= console

QT += core concurrent gui widgets serialport

TEMPLATE = SUBDIRS



SOURCES += \
    baseinterface.cpp \
    ethernet.cpp \
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
    ethernet.h \
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



