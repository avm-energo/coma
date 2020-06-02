#-------------------------------------------------
#
# Project created by QtCreator 2019-06-13T12:18:20
#
#-------------------------------------------------

QMAKE_TARGET_COMPANY = EvelSoft
QMAKE_TARGET_COPYRIGHT = EvelSoft
QMAKE_TARGET_PRODUCT = AVTUK-S
RC_ICONS = ../coma.ico
CONFIG += c++11
VERSION = 0.0.43

QT       += core gui printsupport network serialbus serialport qml

TARGET = AVM-Service
DEFINES += PROGNAME='\\"AVM-Service\\"'
DEFINES += PROGCAPTION='\\"AVM-Service\\040v\\040"$$VERSION"\\040\\"'
DEFINES += COMAVERSION='\\"$$VERSION\\"'
DEFINES += DEVICETYPE=1 # 1 - module, 2 - pribor, for diagnostic messages
DEFINES += PROGSIZE=4 # 1 - SMALL (only for users), 2 - MEDIUM (for mid-class users), 3 - LARGE (for developers of modules), 4 - FULL (for developer of the prog)
TEMPLATE = app


SOURCES += \
    ../gen/logclass.cpp \
    ../modbus/serialport.cpp \
    coma.cpp \
    main.cpp \
    ../check/check.cpp \
    ../check/check84.cpp \
    ../check/checkdialog84.cpp \
    ../check/eabstractcheckdialog.cpp \
    ../config/abstractconfdialog.cpp \
    ../config/abstractconfdialog3x.cpp \
    ../config/confdialog.cpp \
    ../config/confdialog21.cpp \
    ../config/confdialog22.cpp \
    ../config/confdialog31.cpp \
    ../config/confdialog35.cpp \
    ../config/confdialog80.cpp \
    ../config/confdialog84.cpp \
    ../config/confdialog85.cpp \
    ../config/confdialog87.cpp \
    ../config/confdialoga1.cpp \
    ../config/config.cpp \
    ../config/config21.cpp \
    ../config/config22.cpp \
    ../config/config31.cpp \
    ../config/config35.cpp \
    ../config/config3x.cpp \
    ../config/config80.cpp \
    ../config/config84.cpp \
    ../config/config85.cpp \
    ../config/config87.cpp \
    ../config/configa1.cpp \
    ../dialogs/cordialog.cpp \
    ../dialogs/errordialog.cpp \
    ../dialogs/fwupdialog.cpp \
    ../dialogs/infodialog.cpp \
    ../dialogs/journalsdialog.cpp \
    ../dialogs/keypressdialog.cpp \
    ../dialogs/settingsdialog.cpp \
    ../gen/error.cpp \
    ../gen/files.cpp \
    ../gen/mainwindow.cpp \
    ../gen/modulebsi.cpp \
    ../gen/report.cpp \
    ../gen/s2.cpp \
    ../gen/stdfunc.cpp \
    ../gen/timefunc.cpp \
    ../iec104/ethernet.cpp \
    ../iec104/iec104.cpp \
    ../modbus/modbus.cpp \
    ../widgets/ecombobox.cpp \
    ../widgets/emessagebox.cpp \
    ../widgets/errorprotocolwidget.cpp \
    ../widgets/estackedwidget.cpp \
    ../widgets/etableitem.cpp \
    ../widgets/etablemodel.cpp \
    ../widgets/etableview.cpp \
    ../widgets/etabwidget.cpp \
    ../widgets/getoscpbdelegate.cpp \
    ../widgets/lineeditfield.cpp \
    ../widgets/passwordlineedit.cpp \
    ../widgets/signalchoosewidget.cpp \
    ../widgets/waitwidget.cpp \
    ../widgets/wd_func.cpp \
    ../gen/commands.cpp \
    ../gen/eabstractprotocomchannel.cpp \
    ../gen/eusbhid.cpp \
    ../dialogs/time.cpp


HEADERS += \
    ../gen/logclass.h \
    ../modbus/serialport.h \
    coma.h \
    ../check/check.h \
    ../check/check84.h \
    ../check/checkdialog84.h \
    ../check/eabstractcheckdialog.h \
    ../config/abstractconfdialog.h \
    ../config/abstractconfdialog3x.h \
    ../config/confdialog.h \
    ../config/confdialog21.h \
    ../config/confdialog22.h \
    ../config/confdialog31.h \
    ../config/confdialog35.h \
    ../config/confdialog80.h \
    ../config/confdialog84.h \
    ../config/confdialog85.h \
    ../config/confdialog87.h \
    ../config/confdialoga1.h \
    ../config/config.h \
    ../config/config21.h \
    ../config/config22.h \
    ../config/config31.h \
    ../config/config35.h \
    ../config/config3x.h \
    ../config/config80.h \
    ../config/config84.h \
    ../config/config85.h \
    ../config/config87.h \
    ../config/configa1.h \
    ../dialogs/cordialog.h \
    ../dialogs/errordialog.h \
    ../dialogs/fwupdialog.h \
    ../dialogs/infodialog.h \
    ../dialogs/journalsdialog.h \
    ../dialogs/keypressdialog.h \
    ../dialogs/settingsdialog.h \
    ../gen/colors.h \
    ../gen/error.h \
    ../gen/files.h \
    ../gen/maindef.h \
    ../gen/mainwindow.h \
    ../gen/modulebsi.h \
    ../gen/report.h \
    ../gen/s2.h \
    ../gen/stdfunc.h \
    ../gen/timefunc.h \
    ../iec104/ethernet.h \
    ../iec104/iec104.h \
    ../widgets/ecombobox.h \
    ../widgets/emessagebox.h \
    ../widgets/errorprotocolwidget.h \
    ../widgets/estackedwidget.h \
    ../widgets/etableitem.h \
    ../widgets/etablemodel.h \
    ../widgets/etableview.h \
    ../widgets/etabwidget.h \
    ../widgets/getoscpbdelegate.h \
    ../widgets/keypress.h \
    ../widgets/lineeditfield.h \
    ../widgets/passwordlineedit.h \
    ../widgets/signalchoosewidget.h \
    ../widgets/waitwidget.h \
    ../widgets/wd_func.h \
    ../modbus/modbus.h \
    ../gen/commands.h \
    ../gen/eabstractprotocomchannel.h \
    ../gen/eusbhid.h \
    ../dialogs/time.h


INCLUDEPATH += $$PWD/../../includes

equals(QMAKE_PLATFORM, win32)
{
    contains(QMAKE_TARGET.arch, x86_64) {
        message("x64 build")
       ## Windows x64 (64bit) specific build here
        CONFIG(release, debug|release): LIBS += -L$$PWD/../../libs/win64/release/ -llimereport -lliblzma -lhidapi -lqt5xlsx
        CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libs/win64/debug/ -llimereportd -lliblzma -lhidapi -lqt5xlsxd
    } else {
        message("x86 build")
        ## Windows x86 (32bit) specific build here
        CONFIG(release, debug|release): LIBS += -L$$PWD/../../libs/win32/release/ -llimereport -lliblzma -lhidapi -lqt5xlsx
        CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libs/win32/debug/ -llimereportd -lliblzma -lhidapi -lqt5xlsxd
    }
}

unix: LIBS += -L$$PWD/libs/win32/debug/ -llimereportd -lliblzma -lqt5xlsxd

