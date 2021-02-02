include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets concurrent

TARGET = datablock$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += DATABLOCK_LIBRARY


SOURCES += \
    bd0.cpp \
    datablock.cpp \
    kiv/bac.cpp \
    kiv/bda.cpp \
    kiv/bdain.cpp

HEADERS += \
    bd0.h \
    datablock.h \
    kiv/bac.h \
    kiv/bda.h \
    kiv/bdain.h
