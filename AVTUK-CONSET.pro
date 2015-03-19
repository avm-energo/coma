#-------------------------------------------------
#
# Project created by QtCreator 2015-03-02T10:43:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += serialport

TARGET = AVTUK-CONSET
TEMPLATE = app


SOURCES += main.cpp\
        conset.cpp \
    confdialog.cpp \
    tunedialog.cpp \
    checkdialog.cpp \
    fwupdialog.cpp \
    mytabwidget.cpp \
    publicclass.cpp \
    downloaddialog.cpp \
    serialthread.cpp

HEADERS  += conset.h \
    confdialog.h \
    tunedialog.h \
    checkdialog.h \
    fwupdialog.h \
    mytabwidget.h \
    publicclass.h \
    downloaddialog.h \
    serialthread.h
