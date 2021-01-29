include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets concurrent

TARGET = Config$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += CONFIG_LIBRARY

SOURCES += \
    abstractconfdialog.cpp \
    config.cpp \
    configkdv.cpp \
    configkiv.cpp \
    configktf.cpp \
    configkxx.cpp \
    confkdvdialog.cpp \
    confkivdialog.cpp \
    confktfdialog.cpp

HEADERS += \
    abstractconfdialog.h \
    config.h \
    config85.h \
    configkdv.h \
    configkiv.h \
    configktf.h \
    configkxx.h \
    confkdvdialog.h \
    confkivdialog.h \
    confktfdialog.h

