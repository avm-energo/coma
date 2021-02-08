include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets

TARGET = models$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += MODELS_LIBRARY

SOURCES += \
    errorprotocolmodel.cpp \
    etablemodel.cpp \
    etablerow.cpp \
    getoscpbdelegate.cpp

HEADERS += \
    errorprotocolmodel.h \
    etablemodel.h \
    etablerow.h \
    getoscpbdelegate.h
