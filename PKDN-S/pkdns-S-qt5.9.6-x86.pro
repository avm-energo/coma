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
VERSION = 2.2.259

QT       += core gui serialport printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pkdns-S
DEFINES += MODULE_A1
DEFINES += PROGNAME='\\"PKDN-S-S\\"'
DEFINES += PROGCAPTION='\\"PKDN-Service-S\\040v"$$VERSION"\\"'
DEFINES += COMAVERSION='\\"$$VERSION\\"'
DEFINES += DEVICETYPE=2 # 1 - module, 2 - pribor, for diagnostic messages
DEFINES += PROGSIZE=1 # 1 - SMALL (only for users), 2 - MEDIUM (for mid-class users), 3 - LARGE (for developers of modules), 4 - FULL (for developer of the prog)
#DEFINES += COMPORTENABLE # enable virtual com port driver
DEFINES += USBENABLE # enable usb hid driver
TEMPLATE = app

SOURCES += main.cpp\
        pkdn_s.cpp \
    ../gen/eabstractprotocomchannel.cpp \
    ../gen/eusbcom.cpp \
    ../gen/eusbhid.cpp \
    ../gen/commands.cpp \
    ../gen/log.cpp \
    ../gen/mainwindow.cpp \
    ../check/eabstractcheckdialog.cpp \
    ../check/checka1.cpp \
    ../check/checkdialoga1.cpp \
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
    ../widgets/errorprotocolwidget.cpp \
    ../widgets/lineeditfield.cpp \
    ../widgets/emessagebox.cpp \
    ../widgets/estackedwidget.cpp \
    ../widgets/etabwidget.cpp \
    ../widgets/etableitem.cpp \
    ../widgets/etablemodel.cpp \
    ../widgets/ecombobox.cpp \
    ../widgets/etableview.cpp \
    ../widgets/waitwidget.cpp \
    ../widgets/wd_func.cpp \
    ../widgets/passwordlineedit.cpp \
    ../check/check.cpp \
    ../gen/s2.cpp \
    ../gen/error.cpp \
    ../gen/files.cpp \
    ../gen/modulebsi.cpp \
    ../gen/stdfunc.cpp \
    ../gen/timefunc.cpp

HEADERS  += pkdn_s.h \
    ../gen/eabstractprotocomchannel.h \
    ../gen/eusbcom.h \
    ../gen/eusbhid.h \
    ../gen/commands.h \
    ../gen/log.h \
    ../gen/mainwindow.h \
    ../check/eabstractcheckdialog.h \
    ../check/checka1.h \
    ../check/checkdialoga1.h \
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
    ../widgets/errorprotocolwidget.h \
    ../widgets/lineeditfield.h \
    ../widgets/emessagebox.h \
    ../widgets/estackedwidget.h \
    ../widgets/etabwidget.h \
    ../widgets/etableitem.h \
    ../widgets/etablemodel.h \
    ../widgets/ecombobox.h \
    ../widgets/etableview.h \
    ../widgets/waitwidget.h \
    ../widgets/wd_func.h \
    ../widgets/passwordlineedit.h \
    ../check/check.h \
    ../gen/s2.h \
    ../gen/colors.h \
    ../gen/error.h \
    ../gen/files.h \
    ../gen/maindef.h \
    ../gen/modulebsi.h \
    ../gen/stdfunc.h \
    ../gen/timefunc.h

INCLUDEPATH += $$PWD/../../includes

CONFIG(release, debug|release): LIBS += -L$$PWD/../../libs-5.9.6/win32/release/ -llimereport -lliblzma -lhidapi -lqt5xlsx -llibmysql -llibeay32 -lssleay32
CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libs-5.9.6/win32/debug/ -llimereport -lliblzma -lhidapi -lqt5xlsxd -llibmysqld -llibeay32 -lssleay32
