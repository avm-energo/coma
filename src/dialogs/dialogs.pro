include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets serialport concurrent

TARGET = dialogs$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += DIALOGS_LIBRARY

SOURCES += \
    abstractinterfacedialog.cpp \
    connectdialog.cpp \
    errordialog.cpp \
    fwuploaddialog.cpp \
    infodialog.cpp \
    interfaceethernetdialog.cpp \
    interfaceserialdialog.cpp \
    interfaceusbdialog.cpp \
    journalsdialog.cpp \
    keypressdialog.cpp \
    settingsdialog.cpp \
    timedialog.cpp

HEADERS += \
    abstractinterfacedialog.h \
    connectdialog.h \
    errordialog.h \
    fwuploaddialog.h \
    infodialog.h \
    interfaceethernetdialog.h \
    interfaceserialdialog.h \
    interfaceusbdialog.h \
    journalsdialog.h \
    keypressdialog.h \
    settingsdialog.h \
    timedialog.h
