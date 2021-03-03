include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets serialport network

TARGET = interfaces$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += INTERFACES_LIBRARY

INCLUDEPATH += ./../../include/hidapi

include(interfaces.pri)
