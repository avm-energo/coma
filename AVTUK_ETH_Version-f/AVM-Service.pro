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
VERSION = 0.1.5

QT       += core gui printsupport network serialport qml

TARGET = AVM-Service
DEFINES += PROGNAME='\\"AVM-Service\\"'
DEFINES += PROGCAPTION='\\"AVM-Service\\040v\\040"$$VERSION"\\040\\"'
DEFINES += COMAVERSION='\\"$$VERSION\\"'
DEFINES += DEVICETYPE=1 # 1 - module, 2 - pribor, for diagnostic messages
# DEFINES += DEBUG
TEMPLATE = app


SOURCES += \
    ../alarm/abstractalarm.cpp \
    ../alarm/alarmclass.cpp \
    ../alarm/alarmstateall.cpp \
    ../alarm/alarmwidget.cpp \
    ../alarm/avaralarmkiv.cpp \
    ../alarm/avaralarmktf.cpp \
    ../alarm/warnalarmkiv.cpp \
    ../alarm/warnalarmktf.cpp \
    ../check/checkdialogkiv.cpp \
    ../check/checkkiv.cpp \
    ../check/checkktf.cpp \
    ../check/chekdialogktf.cpp \
    ../config/confdialogkdv.cpp \
    ../config/confdialogkiv.cpp \
    ../config/confdialogktf.cpp \
    ../config/configkdv.cpp \
    ../config/configkiv.cpp \
    ../config/configktf.cpp \
    ../dialogs/abstractcordialog.cpp \
    ../dialogs/cordialogktf.cpp \
    ../dialogs/infodialog.cpp \
    ../dialogs/mnktime.cpp \
    ../gen/journals.cpp \
    ../gen/logclass.cpp \
    ../gen/maindef.cpp \
    ../modbus/serialport.cpp \
    coma.cpp \
    main.cpp \
    ../check/check.cpp \
    ../check/eabstractcheckdialog.cpp \
    ../config/abstractconfdialog.cpp \
    ../config/confdialog.cpp \
    ../config/config.cpp \
    ../dialogs/cordialog.cpp \
    ../dialogs/errordialog.cpp \
    ../dialogs/fwupdialog.cpp \
    ../dialogs/journalsdialog.cpp \
    ../dialogs/keypressdialog.cpp \
    ../dialogs/settingsdialog.cpp \
    ../gen/error.cpp \
    ../gen/files.cpp \
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
    ../models/etableitem.cpp \
    ../models/etablemodel.cpp \
    ../widgets/etableview.cpp \
    ../widgets/etabwidget.cpp \
    ../models/getoscpbdelegate.cpp \
    ../widgets/lineeditfield.cpp \
    ../widgets/passwordlineedit.cpp \
    ../widgets/signalchoosewidget.cpp \
    ../widgets/waitwidget.cpp \
    ../widgets/wd_func.cpp \
    ../gen/commands.cpp \
    ../gen/eabstractprotocomchannel.cpp \
    ../gen/eusbhid.cpp


HEADERS += \
    ../alarm/abstractalarm.h \
    ../alarm/alarmclass.h \
    ../alarm/alarmstateall.h \
    ../alarm/alarmwidget.h \
    ../alarm/avaralarmkiv.h \
    ../alarm/avaralarmktf.h \
    ../alarm/warnalarmkiv.h \
    ../alarm/warnalarmktf.h \
    ../check/checkdialogkiv.h \
    ../check/checkkiv.h \
    ../check/checkktf.h \
    ../check/chekdialogktf.h \
    ../config/confdialogkdv.h \
    ../config/confdialogkiv.h \
    ../config/confdialogktf.h \
    ../config/configkdv.h \
    ../config/configkiv.h \
    ../config/configktf.h \
    ../dialogs/abstractcordialog.h \
    ../dialogs/cordialogktf.h \
    ../dialogs/infodialog.h \
    ../dialogs/mnktime.h \
    ../gen/journals.h \
    ../gen/logclass.h \
    ../modbus/serialport.h \
    coma.h \
    ../check/check.h \
    ../check/eabstractcheckdialog.h \
    ../config/abstractconfdialog.h \
    ../config/confdialog.h \
    ../config/config.h \
    ../dialogs/cordialog.h \
    ../dialogs/errordialog.h \
    ../dialogs/fwupdialog.h \
    ../dialogs/journalsdialog.h \
    ../dialogs/keypressdialog.h \
    ../dialogs/settingsdialog.h \
    ../gen/colors.h \
    ../gen/error.h \
    ../gen/files.h \
    ../gen/maindef.h \
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
    ../models/etableitem.h \
    ../models/etablemodel.h \
    ../widgets/etableview.h \
    ../widgets/etabwidget.h \
    ../models/getoscpbdelegate.h \
    ../widgets/keypress.h \
    ../widgets/lineeditfield.h \
    ../widgets/passwordlineedit.h \
    ../widgets/signalchoosewidget.h \
    ../widgets/waitwidget.h \
    ../widgets/wd_func.h \
    ../modbus/modbus.h \
    ../gen/commands.h \
    ../gen/eabstractprotocomchannel.h \
    ../gen/eusbhid.h


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

