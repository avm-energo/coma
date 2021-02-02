include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets serialport concurrent

TARGET = dialogs$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += DIALOGS_LIBRARY

SOURCES += \
    connectdialog.cpp \
    errordialog.cpp \
    fwuploaddialog.cpp \
    infodialog.cpp \
    journalsdialog.cpp \
    keypressdialog.cpp \
    settingsdialog.cpp \
    timedialog.cpp

HEADERS += \
    connectdialog.h \
    errordialog.h \
    fwuploaddialog.h \
    infodialog.h \
    journalsdialog.h \
    keypressdialog.h \
    settingsdialog.h \
    timedialog.h
