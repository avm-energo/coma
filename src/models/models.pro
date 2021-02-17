include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets

TARGET = models$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += MODELS_LIBRARY

SOURCES += \
    errorprotocolitem.cpp \
    errorprotocolmodel.cpp \
    etablemodel.cpp \
    etablerow.cpp

HEADERS += \
    abstracttablerow.h \
    errorprotocolitem.h \
    errorprotocolmodel.h \
    etablemodel.h \
    etablerow.h
