include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets serialport network concurrent

TARGET = General$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += GENERAL_LIBRARY

include(gen.pri)



