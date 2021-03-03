include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets

TARGET = startup$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += STARTUP_LIBRARY

include(startup.pri)

