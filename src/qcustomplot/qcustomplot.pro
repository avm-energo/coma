include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets

TARGET = qcustomplot$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += QCUSTOMPLOT_LIBRARY
DEFINES -= QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    qcustomplot.cpp 

HEADERS += \
    qcustomplot.h 
