#-------------------------------------------------
#
# Project created by QtCreator 2015-03-02T10:43:56
#
#-------------------------------------------------

QT       += core gui network

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
    A/a_checkdialog.cpp \
    A/a_confdialog.cpp \
    A/a_tunedialog.cpp \
    widgets/s_tqcheckbox.cpp \
    widgets/s_tqcombobox.cpp \
    widgets/s_tqspinbox.cpp \
    canal.cpp \
    E/e_checkdialog.cpp \
    E/e_confdialog.cpp \
    E/e_tunedialog.cpp \
    iec104/iec104.cpp \
    iec104/ethernet.cpp

HEADERS  += conset.h \
    fwupdialog.h \
    mytabwidget.h \
    publicclass.h \
    downloaddialog.h \
    serialthread.h \
    A/a_checkdialog.h \
    A/a_confdialog.h \
    A/a_tunedialog.h \
    widgets/s_tqcheckbox.h \
    widgets/s_tqcombobox.h \
    widgets/s_tqspinbox.h \
    config.h \
    canal.h \
    E/e_checkdialog.h \
    E/e_confdialog.h \
    E/e_tunedialog.h \
    iec104/iec104.h \
    iec104/ethernet.h

RESOURCES += \
    res.qrc
