#-------------------------------------------------
#
# Project created by QtCreator 2015-03-02T10:43:56
#
#-------------------------------------------------

QMAKE_TARGET_COMPANY = EvelSoft
QMAKE_TARGET_COPYRIGHT = EvelSoft
QMAKE_TARGET_PRODUCT = AVTUK-S
RC_ICONS = ../coma.ico
CONFIG += c++11
VERSION = 2.2.249

QT       += core gui printsupport

TARGET = avtuks-E
#DEFINES += MODULE_A1
DEFINES += PROGNAME='\\"AVTUK-S\\"'
DEFINES += PROGCAPTION='\\"AVTUK-Service-E\\040v\\040"$$VERSION"\\040\\"'
DEFINES += COMAVERSION='\\"$$VERSION\\"'
DEFINES += DEVICETYPE=1 # 1 - module, 2 - pribor, for diagnostic messages
DEFINES += PROGSIZE=0 # 0 - EMULATION ONLY, 1 - SMALL (only for users), 2 - MEDIUM (for mid-class users), 3 - LARGE (for developers of modules), 4 - FULL (for developer of the prog)
#DEFINES += COMPORTENABLE # enable virtual com port driver
#DEFINES += USBENABLE # enable usb hid driver
TEMPLATE = app

SOURCES += \
    coma.cpp \
    main.cpp \
    ../check/check.cpp \
    ../check/check21.cpp \
    ../check/check22.cpp \
    ../check/check80.cpp \
    ../check/check85.cpp \
    ../check/checka1.cpp \
    ../check/checkdialog21.cpp \
    ../check/checkdialog22.cpp \
    ../check/checkdialog80.cpp \
    ../check/checkdialog85.cpp \
    ../check/checkdialoga1.cpp \
    ../check/eabstractcheckdialog.cpp \
    ../config/abstractconfdialog.cpp \
    ../config/abstractconfdialog3x.cpp \
    ../config/confdialog.cpp \
    ../config/confdialog21.cpp \
    ../config/confdialog22.cpp \
    ../config/confdialog31.cpp \
    ../config/confdialog35.cpp \
    ../config/confdialog80.cpp \
    ../config/confdialog85.cpp \
    ../config/confdialoga1.cpp \
    ../config/config.cpp \
    ../config/config21.cpp \
    ../config/config22.cpp \
    ../config/config31.cpp \
    ../config/config35.cpp \
    ../config/config3x.cpp \
    ../config/config80.cpp \
    ../config/config85.cpp \
    ../config/configa1.cpp \
    ../dialogs/a1dialog.cpp \
    ../dialogs/errordialog.cpp \
    ../dialogs/fwupdialog.cpp \
    ../dialogs/hiddendialog.cpp \
    ../dialogs/infodialog.cpp \
    ../dialogs/keypressdialog.cpp \
    ../dialogs/oscdialog.cpp \
    ../dialogs/settingsdialog.cpp \
    ../dialogs/switchjournaldialog.cpp \
    ../dialogs/swjdialog.cpp \
    ../dialogs/trendviewdialog.cpp \
    ../gen/eoscillogram.cpp \
    ../gen/log.cpp \
    ../gen/mainwindow.cpp \
    ../gen/qcustomplot.cpp \
    ../models/trendviewmodel.cpp \
    ../modules/eabstractmodule.cpp \
    ../tune/eabstracttunedialog.cpp \
    ../tune/tunedialog21.cpp \
    ../tune/tunedialog22.cpp \
    ../tune/tunedialog80.cpp \
    ../tune/tunedialoga1.cpp \
    ../tune/tunedialoga1dn.cpp \
    ../widgets/ecombobox.cpp \
    ../widgets/emessagebox.cpp \
    ../widgets/errorprotocolwidget.cpp \
    ../widgets/estackedwidget.cpp \
    ../widgets/etableitem.cpp \
    ../widgets/etablemodel.cpp \
    ../widgets/etableview.cpp \
    ../widgets/etabwidget.cpp \
    ../widgets/etwoimages.cpp \
    ../widgets/getoscpbdelegate.cpp \
    ../widgets/lineeditfield.cpp \
    ../widgets/passwordlineedit.cpp \
    ../widgets/signalchoosewidget.cpp \
    ../widgets/waitwidget.cpp \
    ../widgets/wd_func.cpp \
    ../gen/s2.cpp \
    ../gen/modulebsi.cpp \
    ../gen/error.cpp \
    ../gen/timefunc.cpp \
    ../gen/files.cpp \
    ../gen/stdfunc.cpp \
    ../tune/tunedialog85.cpp \
    ../config/confdialog87.cpp \
    ../config/config87.cpp

HEADERS  += \
    coma.h \
    ../check/check.h \
    ../check/check21.h \
    ../check/check22.h \
    ../check/check80.h \
    ../check/check85.h \
    ../check/checka1.h \
    ../check/checkdialog21.h \
    ../check/checkdialog22.h \
    ../check/checkdialog80.h \
    ../check/checkdialog85.h \
    ../check/checkdialoga1.h \
    ../check/eabstractcheckdialog.h \
    ../config/abstractconfdialog.h \
    ../config/abstractconfdialog3x.h \
    ../config/confdialog.h \
    ../config/confdialog21.h \
    ../config/confdialog22.h \
    ../config/confdialog31.h \
    ../config/confdialog35.h \
    ../config/confdialog80.h \
    ../config/confdialog85.h \
    ../config/confdialoga1.h \
    ../config/config.h \
    ../config/config21.h \
    ../config/config22.h \
    ../config/config31.h \
    ../config/config35.h \
    ../config/config3x.h \
    ../config/config80.h \
    ../config/config85.h \
    ../config/configa1.h \
    ../dialogs/a1dialog.h \
    ../dialogs/errordialog.h \
    ../dialogs/fwupdialog.h \
    ../dialogs/hiddendialog.h \
    ../dialogs/infodialog.h \
    ../dialogs/keypressdialog.h \
    ../dialogs/oscdialog.h \
    ../dialogs/settingsdialog.h \
    ../dialogs/switchjournaldialog.h \
    ../dialogs/swjdialog.h \
    ../dialogs/trendviewdialog.h \
    ../gen/eoscillogram.h \
    ../gen/log.h \
    ../gen/mainwindow.h \
    ../gen/qcustomplot.h \
    ../models/trendviewmodel.h \
    ../modules/eabstractmodule.h \
    ../tune/eabstracttunedialog.h \
    ../tune/tunedialog21.h \
    ../tune/tunedialog22.h \
    ../tune/tunedialog80.h \
    ../tune/tunedialoga1.h \
    ../tune/tunedialoga1dn.h \
    ../widgets/ecombobox.h \
    ../widgets/emessagebox.h \
    ../widgets/errorprotocolwidget.h \
    ../widgets/estackedwidget.h \
    ../widgets/etableitem.h \
    ../widgets/etablemodel.h \
    ../widgets/etableview.h \
    ../widgets/etabwidget.h \
    ../widgets/etwoimages.h \
    ../widgets/getoscpbdelegate.h \
    ../widgets/keypress.h \
    ../widgets/lineeditfield.h \
    ../widgets/passwordlineedit.h \
    ../widgets/signalchoosewidget.h \
    ../widgets/waitwidget.h \
    ../widgets/wd_func.h \
    ../gen/s2.h \
    ../gen/modulebsi.h \
    ../gen/error.h \
    ../gen/timefunc.h \
    ../gen/files.h \
    ../gen/colors.h \
    ../gen/stdfunc.h \
    ../gen/maindef.h \
    ../tune/tunedialog85.h \
    ../config/confdialog87.h \
    ../config/config87.h

INCLUDEPATH += $$PWD/../../includes

win32 {

    ## Windows common build here

    !contains(QMAKE_TARGET.arch, x86_64) {
        ## Windows x86 (32bit) specific build here
        CONFIG(release, debug|release): LIBS += -L$$PWD/../../libs/win32/release/ -llimereport -lliblzma -lqt5xlsx
        CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libs/win32/debug/ -llimereport -lliblzma -lqt5xlsxd
    } else {
        ## Windows x64 (64bit) specific build here
        CONFIG(release, debug|release): LIBS += -L$$PWD/../../libs/win64/release/ -llimereport -lliblzma -lqt5xlsx
        CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libs/win64/debug/ -llimereport -lliblzma -lqt5xlsxd
    }
}
unix: LIBS += -L$$PWD/libs/win32/debug/ -llimereport -lliblzma -lqt5xlsxd
