include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets

TARGET = datablock$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += DATABLOCK_LIBRARY


include(datablocks.pri)
