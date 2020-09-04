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
VERSION = 0.0.8

QT       += core gui printsupport network qml

TARGET = AVM-Debug
DEFINES += PROGNAME='\\"AVM-Debug\\"'
DEFINES += PROGCAPTION='\\"AVM-Debug\\040v\\040"$$VERSION"\\040\\"'
DEFINES += COMAVERSION='\\"$$VERSION\\"'
DEFINES += DEVICETYPE=1 # 1 - module, 2 - pribor, for diagnostic messages
DEFINES += PROGSIZE=4 # 1 - SMALL (only for users), 2 - MEDIUM (for mid-class users), 3 - LARGE (for developers of modules), 4 - FULL (for developer of the prog)
TEMPLATE = app


SOURCES += \
    ../alarm/abstractalarm.cpp \
    ../alarm/alarmclass.cpp \
    ../alarm/alarmstateall.cpp \
    ../alarm/alarmwidget.cpp \
    ../alarm/avaralarmkiv.cpp \
    ../alarm/warnalarmkiv.cpp \
    ../check/checkkdv.cpp \
    ../check/checkkiv.cpp \
    ../check/checkktf.cpp \
    ../check/checkdialogkdv.cpp \
    ../check/checkdialogkiv.cpp \
    ../check/checkdialogktf.cpp \
    ../config/confdialogkdv.cpp \
    ../config/confdialogkiv.cpp \
    ../config/confdialogktf.cpp \
    ../config/confdialogkxx.cpp \
    ../config/configkdv.cpp \
    ../config/configkiv.cpp \
    ../config/configktf.cpp \
    ../config/configkxx.cpp \
    ../dialogs/mnktime.cpp \
    ../gen/logclass.cpp \
    ../iec104/ethernet.cpp \
    ../iec104/iec104.cpp \
    ../tune/eabstracttunedialog.cpp \
    ../tune/tunedialogkiv.cpp \
    coma.cpp \
    main.cpp \
    ../check/check.cpp \
    ../check/eabstractcheckdialog.cpp \
    ../dialogs/cordialog.cpp \
    ../dialogs/errordialog.cpp \
    ../dialogs/fwupdialog.cpp \
    ../dialogs/infodialog.cpp \
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
    ../gen/maindef.cpp


HEADERS += \
    ../check/checkkdv.h \
    ../check/checkkiv.h \
    ../check/checkktf.h \
    ../check/checkdialogkdv.h \
    ../check/checkdialogkiv.h \
    ../check/checkdialogktf.h \
    ../config/confdialogkdv.h \
    ../config/confdialogkiv.h \
    ../config/confdialogktf.h \
    ../config/configkdv.h \
    ../config/configkiv.h \
    ../config/configktf.h \
    ../dialogs/mnktime.h \
    ../gen/logclass.h \
    ../iec104/ethernet.h \
    ../iec104/iec104.h \
    ../tune/eabstracttunedialog.h \
    ../tune/tunedialogKIV.h \
    coma.h \
    ../check/check.h \
    ../check/eabstractcheckdialog.h \
    ../config/abstractconfdialog.h \
    ../config/confdialog.h \
    ../config/config.h \
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
    ../gen/modulebsi.h \
    ../gen/report.h \
    ../gen/s2.h \
    ../gen/stdfunc.h \
    ../gen/timefunc.h \
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

