#-------------------------------------------------
#
# Project created by QtCreator 2015-03-02T10:43:56
#
#-------------------------------------------------

QT       += core gui testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += serialport

TARGET = AVTUK-CONSET
TEMPLATE = app


SOURCES += main.cpp\
        conset.cpp \
    fwupdialog.cpp \
    mytabwidget.cpp \
    publicclass.cpp \
    downloaddialog.cpp \
    serialthread.cpp \
    A/checkdialog.cpp \
    A/confdialog.cpp \
    A/tunedialog.cpp \
    widgets/s_tqcheckbox.cpp \
    widgets/s_tqcombobox.cpp \
    widgets/s_tqspinbox.cpp

HEADERS  += conset.h \
    fwupdialog.h \
    mytabwidget.h \
    publicclass.h \
    downloaddialog.h \
    serialthread.h \
    A/checkdialog.h \
    A/confdialog.h \
    A/tunedialog.h \
    widgets/s_tqcheckbox.h \
    widgets/s_tqcombobox.h \
    widgets/s_tqspinbox.h
