#-------------------------------------------------
#
# Project created by QtCreator 2015-03-02T10:43:56
#
#-------------------------------------------------

QMAKE_TARGET_COMPANY = EvelSoft
QMAKE_TARGET_COPYRIGHT = EvelSoft
QMAKE_TARGET_PRODUCT = COMA
RC_ICONS = coma.ico
VERSION = 2.0

QT       += core gui network xlsx

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += serialport

TARGET = COMA
TEMPLATE = app


SOURCES += main.cpp\
    canal.cpp \
    coma.cpp \
    downloaddialog.cpp \
    fwupdialog.cpp \
    oscdialog.cpp \
    publicclass.cpp \
    serialthread.cpp \
    check/checkdialog_21.cpp \
    check/checkdialog_80.cpp \
    config/confdialog.cpp \
    config/config21.cpp \
    config/config80.cpp \
    config/confdialog_21.cpp \
    config/confdialog_31.cpp \
    config/confdialog_80.cpp \
    iec104/iec104.cpp \
    iec104/ethernet.cpp \
    threads/waitthread.cpp \
    tune/tunedialog_21.cpp \
    tune/tunedialog_80.cpp \
    widgets/errorprotocolwidget.cpp \
    widgets/getoscpbdelegate.cpp \
    widgets/messagebox.cpp \
    widgets/mytabwidget.cpp \
    widgets/mystackedwidget.cpp \
    widgets/s_tableitem.cpp \
    widgets/s_tablemodel.cpp \
    widgets/s_tqcheckbox.cpp \
    widgets/s_tqcombobox.cpp \
    widgets/s_tqspinbox.cpp \
    widgets/s_tqtableview.cpp \
    widgets/waitwidget.cpp

HEADERS  += coma.h \
    canal.h \
    downloaddialog.h \
    fwupdialog.h \
    oscdialog.h \
    publicclass.h \
    serialthread.h \
    check/checkdialog_21.h \
    check/checkdialog_80.h \
    config.h \
    config/confdialog.h \
    config/confdialog_21.h \
    config/confdialog_31.h \
    config/confdialog_80.h \
    config/config21.h \
    config/config80.h \
    iec104/ethernet.h \
    iec104/iec104.h \
    threads/waitthread.h \
    tune/tunedialog_21.h \
    tune/tunedialog_80.h \
    widgets/getoscpbdelegate.h \
    widgets/mytabwidget.h \
    widgets/errorprotocolwidget.h \
    widgets/messagebox.h \
    widgets/mystackedwidget.h \
    widgets/s_tableitem.h \
    widgets/s_tablemodel.h \
    widgets/s_tqcheckbox.h \
    widgets/s_tqcombobox.h \
    widgets/s_tqspinbox.h \
    widgets/s_tqtableview.h \
    widgets/waitwidget.h

INCLUDEPATH += $$PWD/../../xlsxwriter/src/xlsx
DEPENDPATH += $$PWD/../../xlsxwriter/src/xlsx

RESOURCES += \
    res.qrc
