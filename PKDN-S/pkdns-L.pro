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
VERSION = 2.1.235

QT       += core gui serialport printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pkdns-L
DEFINES += MODULE_A1
DEFINES += PROGNAME='\\"PKDN-S-L\\"'
DEFINES += PROGCAPTION='\\"PKDN-Service-L\\040v"$$VERSION"\\"'
DEFINES += COMAVERSION='\\"$$VERSION\\"'
DEFINES += DEVICETYPE=2 # 1 - module, 2 - pribor, for diagnostic messages
DEFINES += PROGSIZE=3 # 1 - SMALL (only for users), 2 - MEDIUM (for mid-class users), 3 - LARGE (for developers of modules), 4 - FULL (for developer of the prog)
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
    ../dialogs/fwupdialog.cpp \
    ../dialogs/hiddendialog.cpp \
    ../dialogs/infodialog.cpp \
    ../dialogs/keypressdialog.cpp \
    ../dialogs/settingsdialog.cpp \
    ../gen/eabstractprotocomchannel.cpp \
    ../gen/eusbcom.cpp \
    ../gen/eusbhid.cpp \
    ../gen/commands.cpp \
    ../gen/log.cpp \
    ../gen/mainwindow.cpp \
    ../gen/publicclass.cpp \
    ../tune/eabstracttunedialog.cpp \
    ../tune/tunedialoga1.cpp \
    ../tune/tunedialoga1dn.cpp \
    ../widgets/ecombobox.cpp \
    ../widgets/errorprotocolwidget.cpp \
    ../widgets/estackedwidget.cpp \
    ../widgets/etableitem.cpp \
    ../widgets/etablemodel.cpp \
    ../widgets/etableview.cpp \
    ../widgets/etabwidget.cpp \
    ../widgets/lineeditfield.cpp \
    ../widgets/emessagebox.cpp \
    ../widgets/waitwidget.cpp \
    ../widgets/wd_func.cpp \
    ../widgets/passwordlineedit.cpp \
    ../gen/s2.cpp

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
    ../dialogs/fwupdialog.h \
    ../dialogs/hiddendialog.h \
    ../dialogs/infodialog.h \
    ../dialogs/keypressdialog.h \
    ../dialogs/settingsdialog.h \
    ../gen/eabstractprotocomchannel.h \
    ../gen/eusbcom.h \
    ../gen/eusbhid.h \
    ../gen/commands.h \
    ../gen/log.h \
    ../gen/mainwindow.h \
    ../gen/publicclass.h \
    ../tune/eabstracttunedialog.h \
    ../tune/tunedialoga1.h \
    ../tune/tunedialoga1dn.h \
    ../widgets/ecombobox.h \
    ../widgets/errorprotocolwidget.h \
    ../widgets/estackedwidget.h \
    ../widgets/etableitem.h \
    ../widgets/etablemodel.h \
    ../widgets/etableview.h \
    ../widgets/etabwidget.h \
    ../widgets/lineeditfield.h \
    ../widgets/emessagebox.h \
    ../widgets/waitwidget.h \
    ../widgets/wd_func.h \
    ../widgets/passwordlineedit.h \
    ../gen/s2.h

INCLUDEPATH += $$PWD/../../includes

win32 {

    ## Windows common build here

    !contains(QMAKE_TARGET.arch, x86_64) {
        ## Windows x86 (32bit) specific build here
        CONFIG(release, debug|release): LIBS += -L$$PWD/../../libs/win32/release/ -llimereport -lliblzma -lhidapi -lqt5xlsx
        CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libs/win32/debug/ -llimereport -lliblzma -lhidapi -lqt5xlsxd
    } else {
        ## Windows x64 (64bit) specific build here
        CONFIG(release, debug|release): LIBS += -L$$PWD/../../libs/win64/release/ -llimereport -lliblzma -lhidapi -lqt5xlsx
        CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libs/win64/debug/ -llimereport -lliblzma -lhidapi -lqt5xlsxd
    }
}
unix: LIBS += -L$$PWD/libs/win32/debug/ -llimereport -lliblzma -lhidapi -lqt5xlsxd
