include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets

TARGET = avtuk$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += AVTUK_LIBRARY

SOURCES += \
    eoscillogram.cpp \
    getoscpbdelegate.cpp \
    oscdialog.cpp \
    oscmanager.cpp \
    parseid10031.cpp \
    parseid9000.cpp \
    parseid9050.cpp \
    parsemodule.cpp \
    switchjournaldialog.cpp \
    swjdialog.cpp \
    trendviewdialog.cpp \
    trendviewmodel.cpp

HEADERS += \
    eoscillogram.h \
    getoscpbdelegate.h \
    oscdialog.h \
    oscmanager.h \
    parseid10031.h \
    parseid9000.h \
    parseid9050.h \
    parsemodule.h \
    switchjournaldialog.h \
    swjdialog.h \
    trendviewdialog.h \
    trendviewmodel.h
