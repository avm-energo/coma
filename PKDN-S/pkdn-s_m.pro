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
VERSION = 1.0.72

QT       += core gui serialport printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pkdn-s_m
DEFINES += MODULE_TYPEB=0xA1
DEFINES += PROGNAME='\\"PKDN-S\\"'
DEFINES += PROGCAPTION='\\"PKDN-Service-S\\040v1.0.72\\"'
DEFINES += DEVICETYPE=2 # 1 - module, 2 - pribor, for diagnostic messages
DEFINES += PROGSIZE=2 # 1 - SMALL (only for users), 2 - MEDIUM (for mid-class users), 3 - LARGE (for developers of modules), 4 - FULL (for developer of the prog)
DEFINES += COMPORTENABLE # enable virtual com port driver
#DEFINES += USBENABLE # enable usb hid driver
TEMPLATE = app

SOURCES += main.cpp\
        pkdn_s.cpp \
    ../eabstractprotocomchannel.cpp \
    ../eusbcom.cpp \
    ../eusbhid.cpp \
    ../commands.cpp \
    ../log.cpp \
    ../mainwindow.cpp \
    ../publicclass.cpp \
    ../report.cpp \
    ../check/abstractcheckdialog.cpp \
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
    ../tune/eabstracttunedialog.cpp \
    ../tune/tunedialoga1dn.cpp \
    ../widgets/errorprotocolwidget.cpp \
    ../widgets/getoscpbdelegate.cpp \
    ../widgets/lineeditfield.cpp \
    ../widgets/messagebox.cpp \
    ../widgets/mystackedwidget.cpp \
    ../widgets/mytabwidget.cpp \
    ../widgets/etableitem.cpp \
    ../widgets/etablemodel.cpp \
    ../widgets/s_tqcheckbox.cpp \
    ../widgets/s_tqcombobox.cpp \
    ../widgets/s_tqspinbox.cpp \
    ../widgets/s_tqtableview.cpp \
    ../widgets/waitwidget.cpp \
    ../widgets/wd_func.cpp

HEADERS  += pkdn_s.h \
    ../eabstractprotocomchannel.h \
    ../eusbcom.h \
    ../eusbhid.h \
    ../commands.h \
    ../log.h \
    ../mainwindow.h \
    ../publicclass.h \
    ../report.h \
    ../check/abstractcheckdialog.h \
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
    ../tune/eabstracttunedialog.h \
    ../tune/tunedialoga1dn.h \
    ../widgets/errorprotocolwidget.h \
    ../widgets/getoscpbdelegate.h \
    ../widgets/lineeditfield.h \
    ../widgets/messagebox.h \
    ../widgets/mystackedwidget.h \
    ../widgets/mytabwidget.h \
    ../widgets/etableitem.h \
    ../widgets/etablemodel.h \
    ../widgets/s_tqcheckbox.h \
    ../widgets/s_tqcombobox.h \
    ../widgets/s_tqspinbox.h \
    ../widgets/s_tqtableview.h \
    ../widgets/waitwidget.h \
    ../widgets/wd_func.h

RESOURCES += \
    pkdn-s.qrc

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
