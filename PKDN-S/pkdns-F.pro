#-------------------------------------------------
#
# Project created by QtCreator 2017-07-07T16:10:41
#
#-------------------------------------------------

QMAKE_TARGET_COMPANY = AVM-Energo
QMAKE_TARGET_COPYRIGHT = AVM-Energo
QMAKE_TARGET_PRODUCT = PKDN-S
RC_ICONS = ../coma.ico
CONFIG += c++11
VERSION = 2.0.0

QT       += core gui serialport printsupport qml widgets xml sql

TARGET = pkdns-F
DEFINES += MODULE_A1
DEFINES += COMAVERSION='\\"2.0.0\\"'
DEFINES += PROGNAME='\\"PKDN-S\\"'
DEFINES += PROGCAPTION='\\"PKDN\\040Service\\040F\\040v2.0.0\\"'
DEFINES += DEVICETYPE=2 # 1 - module, 2 - pribor, for diagnostic messages
DEFINES += PROGSIZE=4 # 1 - SMALL (only for users), 2 - MEDIUM (for mid-class users), 3 - LARGE (for developers of modules), 4 - FULL (for developer of the prog)
#DEFINES += COMPORTENABLE # enable virtual com port driver
DEFINES += USBENABLE # enable usb hid driver
TEMPLATE = app

SOURCES += main.cpp\
        pkdn_s.cpp \
    ../gen/commands.cpp \
    ../gen/eabstractprotocomchannel.cpp \
    ../gen/error.cpp \
    ../gen/eusbcom.cpp \
    ../gen/eusbhid.cpp \
    ../gen/files.cpp \
    ../gen/log.cpp \
    ../gen/mainwindow.cpp \
    ../gen/modulebsi.cpp \
    ../gen/report.cpp \
    ../gen/s2.cpp \
    ../gen/stdfunc.cpp \
    ../check/eabstractcheckdialog.cpp \
    ../check/check.cpp \
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
    ../dialogs/fwupdialog.cpp \
    ../dialogs/hiddendialog.cpp \
    ../dialogs/keypressdialog.cpp \
    ../dialogs/settingsdialog.cpp \
    ../tune/eabstracttunedialoga1dn.cpp \
    ../tune/eabstracttunedialog.cpp \
    ../tune/tunedialoga1.cpp \
    ../tune/tunedialoga1dn.cpp \
    ../widgets/errorprotocolwidget.cpp \
    ../widgets/etableitem.cpp \
    ../widgets/etablemodel.cpp \
    ../widgets/lineeditfield.cpp \
    ../widgets/emessagebox.cpp \
    ../widgets/estackedwidget.cpp \
    ../widgets/etabwidget.cpp \
    ../widgets/ecombobox.cpp \
    ../widgets/etableview.cpp \
    ../widgets/passwordlineedit.cpp \
    ../widgets/waitwidget.cpp \
    ../widgets/wd_func.cpp

HEADERS  += pkdn_s.h \
    ../gen/commands.h \
    ../gen/eabstractprotocomchannel.h \
    ../gen/error.h \
    ../gen/eusbcom.h \
    ../gen/eusbhid.h \
    ../gen/files.h \
    ../gen/log.h \
    ../gen/stdfunc.h \
    ../gen/mainwindow.h \
    ../gen/modulebsi.h \
    ../gen/report.h \
    ../gen/s2.h \
    ../check/eabstractcheckdialog.h \
    ../check/check.h \
    ../check/checka1.h \
    ../check/checkdialoga1.h \
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
    ../tune/eabstracttunedialog.h \
    ../tune/eabstracttunedialoga1dn.h \
    ../tune/tunedialoga1.h \
    ../tune/tunedialoga1dn.h \
    ../widgets/errorprotocolwidget.h \
    ../widgets/etableitem.h \
    ../widgets/etablemodel.h \
    ../widgets/lineeditfield.h \
    ../widgets/emessagebox.h \
    ../widgets/estackedwidget.h \
    ../widgets/etabwidget.h \
    ../widgets/ecombobox.h \
    ../widgets/etableview.h \
    ../widgets/passwordlineedit.h \
    ../widgets/waitwidget.h \
    ../widgets/wd_func.h

# RESOURCES += \
    # pkdn-s.qrc

INCLUDEPATH += $$PWD/../../includes

win32 {

    ## Windows common build here

    !contains(QMAKE_TARGET.arch, x86_64) {
        ## Windows x86 (32bit) specific build here
        CONFIG(release, debug|release): LIBS += -L$$PWD/../../libs/win32/release/ -llimereport -llzma -lhidapi -lqxlsx
        CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libs/win32/debug/ -llimereport -llzma -lhidapi -lqxlsx
    } else {
        ## Windows x64 (64bit) specific build here
        CONFIG(release, debug|release): LIBS += -L$$PWD/../../libs/win64/release/ -llimereport -llzma -lhidapi -lqxlsx
        CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libs/win64/debug/ -llimereport -llzma -lhidapi -lqxlsx
    }
}
unix: LIBS += -L$$PWD/libs/win32/debug/ -llimereport -llzma -lhidapi -lqxlsx
