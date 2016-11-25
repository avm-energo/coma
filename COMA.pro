#-------------------------------------------------
#
# Project created by QtCreator 2015-03-02T10:43:56
#
#-------------------------------------------------

QMAKE_TARGET_COMPANY = EvelSoft
QMAKE_TARGET_COPYRIGHT = EvelSoft
QMAKE_TARGET_PRODUCT = COMA
RC_ICONS = coma.ico
VERSION = 1.0.0075

QT       += core gui network xlsx

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += serialport

TARGET = COMA
TEMPLATE = app


SOURCES += main.cpp\
        coma.cpp \
    fwupdialog.cpp \
    publicclass.cpp \
    downloaddialog.cpp \
    serialthread.cpp \
    A/a_checkdialog.cpp \
    A/a_tunedialog.cpp \
    widgets/s_tqcheckbox.cpp \
    widgets/s_tqcombobox.cpp \
    widgets/s_tqspinbox.cpp \
    canal.cpp \
    E/e_checkdialog.cpp \
    E/e_tunedialog.cpp \
    iec104/iec104.cpp \
    iec104/ethernet.cpp \
    widgets/s_tqtableview.cpp \
    oscdialog.cpp \
    widgets/s_tablemodel.cpp \
    widgets/s_tableitem.cpp \
    widgets/getoscpbdelegate.cpp \
    D/d_confdialog.cpp \
    widgets/mytabwidget.cpp \
    widgets/errorprotocolwidget.cpp \
    widgets/mystackedwidget.cpp \
    widgets/waitwidget.cpp \
    threads/waitthread.cpp \
    config/config21.cpp \
    config/confdialog_2x.cpp \
    config/confdialog_80.cpp \
    config/config80.cpp \
    config/confdialog.cpp \
    widgets/messagebox.cpp \
    config/confdialog_3x.cpp \
    check/checkdialog_2x.cpp \
    check/checkdialog_80.cpp \
    check/checkdialog_21.cpp \
    config/confdialog_21.cpp \
    config/confdialog_31.cpp \
    tune/tunedialog_21.cpp \
    tune/tunedialog_80.cpp

HEADERS  += coma.h \
    fwupdialog.h \
    publicclass.h \
    downloaddialog.h \
    serialthread.h \
    A/a_checkdialog.h \
    widgets/s_tqcheckbox.h \
    widgets/s_tqcombobox.h \
    widgets/s_tqspinbox.h \
    canal.h \
    E/e_checkdialog.h \
    E/e_tunedialog.h \
    iec104/iec104.h \
    iec104/ethernet.h \
    widgets/s_tqtableview.h \
    oscdialog.h \
    widgets/s_tableitem.h \
    widgets/s_tablemodel.h \
    widgets/getoscpbdelegate.h \
    widgets/mytabwidget.h \
    widgets/errorprotocolwidget.h \
    widgets/mystackedwidget.h \
    widgets/waitwidget.h \
    threads/waitthread.h \
    config.h \
    config/config21.h \
    config/confdialog_2x.h \
    config/confdialog_80.h \
    config/config80.h \
    config/confdialog.h \
    widgets/messagebox.h \
    config/confdialog_3x.h \
    check/checkdialog_2x.h \
    check/checkdialog_80.h \
    check/checkdialog_21.h \
    config/confdialog_21.h \
    config/confdialog_31.h \
    tune/tunedialog_21.h \
    tune/tunedialog_80.h

INCLUDEPATH += $$PWD/../../xlsxwriter/src/xlsx
DEPENDPATH += $$PWD/../../xlsxwriter/src/xlsx

RESOURCES += \
    res.qrc
