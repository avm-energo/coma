include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets

TARGET = avtuk$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += AVTUK_LIBRARY

include(avtuk.pri)
