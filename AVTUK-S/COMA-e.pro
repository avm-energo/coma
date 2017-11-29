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
VERSION = 2.0.82

QT       += core gui printsupport

TARGET = avtuk-s_e
#DEFINES += MODULE_TYPEB=0xA1
DEFINES += PROGNAME='\\"AVTUK-S\\"'
DEFINES += PROGCAPTION='\\"AVTUK-Service-E\\040v2.0.82\\"'
DEFINES += DEVICETYPE=1 # 1 - module, 2 - pribor, for diagnostic messages
DEFINES += PROGSIZE=0 # 0 - EMULATION ONLY, 1 - SMALL (only for users), 2 - MEDIUM (for mid-class users), 3 - LARGE (for developers of modules), 4 - FULL (for developer of the prog)
#DEFINES += COMPORTENABLE # enable virtual com port driver
#DEFINES += USBENABLE # enable usb hid driver
TEMPLATE = app

SOURCES += \
    main.cpp\
    coma.cpp \
    ../check/check21.cpp \
    ../check/check80.cpp \
    ../check/checka1.cpp \
    ../check/checkdialog21.cpp \
    ../check/checkdialog80.cpp \
    ../check/checkdialoga1.cpp \
    ../check/eabstractcheckdialog.cpp \
    ../config/abstractconfdialog.cpp \
    ../config/abstractconfdialog2x.cpp \
    ../config/abstractconfdialog3x.cpp \
    ../config/confdialog.cpp \
    ../config/confdialog21.cpp \
    ../config/confdialog31.cpp \
    ../config/confdialog35.cpp \
    ../config/confdialog80.cpp \
    ../config/confdialog85.cpp \
    ../config/confdialoga1.cpp \
    ../config/config.cpp \
    ../config/config21.cpp \
    ../config/config2x.cpp \
    ../config/config31.cpp \
    ../config/config35.cpp \
    ../config/config3x.cpp \
    ../config/config80.cpp \
    ../config/config85.cpp \
    ../config/configa1.cpp \
    ../dialogs/a1dialog.cpp \
    ../dialogs/errordialog.cpp \
    ../dialogs/infodialog.cpp \
    ../dialogs/settingsdialog.cpp \
    ../gen/commands.cpp \
    ../gen/log.cpp \
    ../gen/mainwindow.cpp \
    ../gen/publicclass.cpp \
    ../widgets/errorprotocolwidget.cpp \
    ../widgets/getoscpbdelegate.cpp \
    ../widgets/messagebox.cpp \
    ../widgets/mytabwidget.cpp \
    ../widgets/mystackedwidget.cpp \
    ../widgets/etableitem.cpp \
    ../widgets/etablemodel.cpp \
    ../widgets/s_tqcombobox.cpp \
    ../widgets/s_tqtableview.cpp \
    ../widgets/waitwidget.cpp \
    ../widgets/wd_func.cpp \ 
    ../check/check.cpp \
    ../check/check85.cpp \
    ../check/checkdialog85.cpp \
    ../dialogs/oscdialog.cpp \
    ../dialogs/trendviewdialog.cpp \
    ../gen/qcustomplot.cpp \
    ../gen/eoscillogram.cpp

HEADERS  += \
    coma.h \
    ../check/check21.h \
    ../check/check80.h \
    ../check/checka1.h \
    ../check/checkdialog21.h \
    ../check/checkdialog80.h \
    ../check/checkdialoga1.h \
    ../check/eabstractcheckdialog.h \
    ../config/abstractconfdialog.h \
    ../config/abstractconfdialog2x.h \
    ../config/abstractconfdialog3x.h \
    ../config/confdialog.h \
    ../config/confdialog21.h \
    ../config/confdialog31.h \
    ../config/confdialog35.h \
    ../config/confdialog80.h \
    ../config/confdialog85.h \
    ../config/confdialoga1.h \
    ../config/config.h \
    ../config/config21.h \
    ../config/config2x.h \
    ../config/config31.h \
    ../config/config35.h \
    ../config/config3x.h \
    ../config/config80.h \
    ../config/config85.h \
    ../config/configa1.h \
    ../dialogs/a1dialog.h \
    ../dialogs/errordialog.h \
    ../dialogs/infodialog.h \
    ../dialogs/settingsdialog.h \
    ../gen/commands.h \
    ../gen/log.h \
    ../gen/mainwindow.h \
    ../gen/publicclass.h \
    ../widgets/getoscpbdelegate.h \
    ../widgets/mytabwidget.h \
    ../widgets/errorprotocolwidget.h \
    ../widgets/messagebox.h \
    ../widgets/mystackedwidget.h \
    ../widgets/etableitem.h \
    ../widgets/etablemodel.h \
    ../widgets/s_tqcombobox.h \
    ../widgets/s_tqtableview.h \
    ../widgets/waitwidget.h \
    ../widgets/wd_func.h \
    ../check/check.h \
    ../check/check85.h \
    ../check/checkdialog85.h \
    ../dialogs/oscdialog.h \
    ../dialogs/trendviewdialog.h \
    ../gen/qcustomplot.h \
    ../gen/eoscillogram.h

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
