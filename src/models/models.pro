include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets

TARGET = models$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += MODELS_LIBRARY

SOURCES += \
    errorprotocolmodel.cpp \
    etableitem.cpp \
    etablemodel.cpp \
    getoscpbdelegate.cpp

HEADERS += \
    errorprotocolmodel.h \
    etableitem.h \
    etablemodel.h \
    getoscpbdelegate.h
