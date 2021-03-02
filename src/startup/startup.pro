include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets

TARGET = startup$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += STARTUP_LIBRARY

SOURCES += \
    abstractstartupdialog.cpp \
    startupkdvdialog.cpp \
    startupkivdialog.cpp \
    startupktfdialog.cpp


HEADERS += \
    abstractstartupdialog.h \
    startupkdvdialog.h \
    startupkivdialog.h \
    startupktfdialog.h

