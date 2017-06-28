#-------------------------------------------------
#
# Project created by QtCreator 2015-03-02T10:43:56
#
#-------------------------------------------------

QMAKE_TARGET_COMPANY = EvelSoft
QMAKE_TARGET_COPYRIGHT = EvelSoft
QMAKE_TARGET_PRODUCT = COMA
RC_ICONS = coma.ico
CONFIG += c++11
VERSION = 2.0

QT       += widgets core gui network serialport

TARGET = COMA
TEMPLATE = app

LIBS += -lQt5Xlsx

SOURCES += main.cpp\
    canal.cpp \
    coma.cpp \
    publicclass.cpp \
    check/checkdialog21.cpp \
    check/checkdialog80.cpp \
    iec104/iec104.cpp \
    iec104/ethernet.cpp \
    threads/waitthread.cpp \
    tune/tunedialog21.cpp \
    tune/tunedialog80.cpp \
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
    widgets/waitwidget.cpp \
    commands.cpp \
    dialogs/downloaddialog.cpp \
    dialogs/fwupdialog.cpp \
    dialogs/oscdialog.cpp \
    dialogs/hiddendialog.cpp \
    widgets/wd_func.cpp \
    log.cpp \
    dialogs/settingsdialog.cpp \
    dialogs/errordialog.cpp \
    config/config2x.cpp \
    config/config.cpp \
    config/confdialog21.cpp \
    config/abstractconfdialog.cpp \
    config/abstractconfdialog2x.cpp \
    config/config21.cpp \
    config/confdialog.cpp \
    config/abstractconfdialog3x.cpp \
    config/config3x.cpp \
    config/confdialog31.cpp \
    config/config31.cpp \
    config/confdialog80.cpp \
    config/config80.cpp \
    check/abstractcheckdialog.cpp

HEADERS  += coma.h \
    canal.h \
    publicclass.h \
    check/checkdialog21.h \
    check/checkdialog80.h \
    config/config80.h \
    iec104/ethernet.h \
    iec104/iec104.h \
    threads/waitthread.h \
    tune/tunedialog21.h \
    tune/tunedialog80.h \
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
    widgets/waitwidget.h \
    commands.h \
    dialogs/downloaddialog.h \
    dialogs/fwupdialog.h \
    dialogs/oscdialog.h \
    dialogs/hiddendialog.h \
    config/config.h \
    widgets/wd_func.h \
    log.h \
    dialogs/settingsdialog.h \
    dialogs/errordialog.h \
    config/config2x.h \
    config/confdialog21.h \
    config/abstractconfdialog.h \
    config/abstractconfdialog2x.h \
    config/config21.h \
    config/confdialog.h \
    config/abstractconfdialog3x.h \
    config/config3x.h \
    config/confdialog31.h \
    config/config31.h \
    config/confdialog80.h \
    config/config80.h \
    check/abstractcheckdialog.h

RESOURCES += \
    res.qrc
