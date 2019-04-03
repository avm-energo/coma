#-------------------------------------------------
#
# Project created by QtCreator 2017-07-07T16:10:41
#
#-------------------------------------------------

QMAKE_TARGET_COMPANY = EvelSoft
QMAKE_TARGET_COPYRIGHT = EvelSoft
QMAKE_TARGET_PRODUCT = PKDN-S
RC_ICONS = ../coma.ico
CONFIG += c++11
VERSION = 2.2.265

QT       += core gui serialport printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pkdns-M
DEFINES += MODULE_A1
DEFINES += PROGNAME='\\"PKDN-S-M\\"'
DEFINES += PROGCAPTION='\\"PKDN-Service-M\\040v"$$VERSION"\\"'
DEFINES += COMAVERSION='\\"$$VERSION\\"'
DEFINES += DEVICETYPE=2 # 1 - module, 2 - pribor, for diagnostic messages
DEFINES += PROGSIZE=2 # 1 - SMALL (only for users), 2 - MEDIUM (for mid-class users), 3 - LARGE (for developers of modules), 4 - FULL (for developer of the prog)
#DEFINES += COMPORTENABLE # enable virtual com port driver
DEFINES += USBENABLE # enable usb hid driver
TEMPLATE = app

SOURCES += main.cpp\
        pkdn_s.cpp \
    ../check/check.cpp \
    ../check/checka1.cpp \
    ../check/checkdialoga1.cpp \
    ../check/eabstractcheckdialog.cpp \
    ../config/abstractconfdialog.cpp \
    ../config/confdialog.cpp \
    ../config/confdialoga1.cpp \
    ../config/config.cpp \
    ../config/configa1.cpp \
    ../dialogs/a1dialog.cpp \
    ../dialogs/errordialog.cpp \
    ../dialogs/infodialog.cpp \
    ../dialogs/keypressdialog.cpp \
    ../dialogs/settingsdialog.cpp \
    ../gen/commands.cpp \
    ../gen/eabstractprotocomchannel.cpp \
    ../gen/error.cpp \
    ../gen/eusbcom.cpp \
    ../gen/eusbhid.cpp \
    ../gen/files.cpp \
    ../gen/log.cpp \
    ../gen/mainwindow.cpp \
    ../gen/modulebsi.cpp \
    ../gen/s2.cpp \
    ../gen/stdfunc.cpp \
    ../gen/timefunc.cpp \
    ../tune/eabstracttunedialog.cpp \
    ../tune/tunedialoga1dn.cpp \
    ../widgets/emessagebox.cpp \
    ../widgets/errorprotocolwidget.cpp \
    ../widgets/estackedwidget.cpp \
    ../widgets/etabwidget.cpp \
    ../widgets/etableitem.cpp \
    ../widgets/etablemodel.cpp \
    ../widgets/ecombobox.cpp \
    ../widgets/etableview.cpp \
    ../widgets/lineeditfield.cpp \
    ../widgets/passwordlineedit.cpp \
    ../widgets/waitwidget.cpp \
    ../widgets/wd_func.cpp \
    ../tune/eabstracttunedialoga1dn.cpp \
    ../gen/report.cpp

HEADERS  += pkdn_s.h \
    ../check/check.h \
    ../check/checka1.h \
    ../check/checkdialoga1.h \
    ../check/eabstractcheckdialog.h \
    ../config/abstractconfdialog.h \
    ../config/confdialog.h \
    ../config/confdialoga1.h \
    ../config/config.h \
    ../config/configa1.h \
    ../dialogs/a1dialog.h \
    ../dialogs/errordialog.h \
    ../dialogs/infodialog.h \
    ../dialogs/keypressdialog.h \
    ../dialogs/settingsdialog.h \
    ../gen/colors.h \
    ../gen/commands.h \
    ../gen/eabstractprotocomchannel.h \
    ../gen/error.h \
    ../gen/eusbcom.h \
    ../gen/eusbhid.h \
    ../gen/files.h \
    ../gen/log.h \
    ../gen/maindef.h \
    ../gen/mainwindow.h \
    ../gen/modulebsi.h \
    ../gen/s2.h \
    ../gen/stdfunc.h \
    ../gen/timefunc.h \
    ../tune/eabstracttunedialog.h \
    ../tune/tunedialoga1dn.h \
    ../widgets/errorprotocolwidget.h \
    ../widgets/emessagebox.h \
    ../widgets/estackedwidget.h \
    ../widgets/etabwidget.h \
    ../widgets/etableitem.h \
    ../widgets/etablemodel.h \
    ../widgets/ecombobox.h \
    ../widgets/etableview.h \
    ../widgets/lineeditfield.h \
    ../widgets/passwordlineedit.h \
    ../widgets/waitwidget.h \
    ../widgets/wd_func.h \
    ../tune/eabstracttunedialoga1dn.h \
    ../gen/report.h

INCLUDEPATH += $$PWD/../../includes

CONFIG(release, debug|release): LIBS += -L$$PWD/../../libs-5.9.6/win32/release/ -llimereport -lliblzma -lhidapi -lqt5xlsx
CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libs-5.9.6/win32/debug/ -llimereport -lliblzma -lhidapi -lqt5xlsxd