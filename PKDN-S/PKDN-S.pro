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
VERSION = 1.0.52

QT       += core gui xlsx serialport printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PKDN-S
DEFINES += PROGNAME='\\"PKDN-S\\"'
DEFINES += PROGCAPTION='\\"PKDN-Service\\040v1.0.52\\"'
TEMPLATE = app

SOURCES += main.cpp\
        pkdn_s.cpp \
    ../canal.cpp \
    ../commands.cpp \
    ../log.cpp \
    ../publicclass.cpp \
    ../widgets/errorprotocolwidget.cpp \
    ../widgets/getoscpbdelegate.cpp \
    ../widgets/messagebox.cpp \
    ../widgets/mystackedwidget.cpp \
    ../widgets/mytabwidget.cpp \
    ../widgets/s_tableitem.cpp \
    ../widgets/s_tablemodel.cpp \
    ../widgets/s_tqcheckbox.cpp \
    ../widgets/s_tqcombobox.cpp \
    ../widgets/s_tqspinbox.cpp \
    ../widgets/s_tqtableview.cpp \
    ../widgets/waitwidget.cpp \
    ../widgets/wd_func.cpp \
    ../dialogs/errordialog.cpp \
    ../dialogs/fwupdialog.cpp \
    ../dialogs/hiddendialog.cpp \
    ../dialogs/oscdialog.cpp \
    ../check/abstractcheckdialog.cpp \
    ../config/abstractconfdialog.cpp \
    ../config/confdialog.cpp \
    ../config/confdialoga1.cpp \
    ../config/config.cpp \
    ../config/configa1.cpp \
    ../check/checkdialoga1.cpp \
    ../tune/tunedialoga1.cpp \
    ../dialogs/keypressdialog.cpp \
    ../check/checka1.cpp \
    ../dialogs/settingsdialog.cpp \
    ../dialogs/downloaddialog.cpp \
    ../tune/abstracttunedialog.cpp \
    ../tune/tunedialoga1dn.cpp \
    ../dialogs/a1dialog.cpp \
    ../report.cpp \
    ../mainwindow.cpp \
    ../dialogs/infodialog.cpp \
    ../widgets/lineeditfield.cpp

HEADERS  += pkdn_s.h \
    ../Canal.h \
    ../commands.h \
    ../log.h \
    ../publicclass.h \
    ../widgets/errorprotocolwidget.h \
    ../widgets/getoscpbdelegate.h \
    ../widgets/messagebox.h \
    ../widgets/mystackedwidget.h \
    ../widgets/mytabwidget.h \
    ../widgets/s_tableitem.h \
    ../widgets/s_tablemodel.h \
    ../widgets/s_tqcheckbox.h \
    ../widgets/s_tqcombobox.h \
    ../widgets/s_tqspinbox.h \
    ../widgets/s_tqtableview.h \
    ../widgets/waitwidget.h \
    ../widgets/wd_func.h \
    ../dialogs/errordialog.h \
    ../dialogs/fwupdialog.h \
    ../dialogs/hiddendialog.h \
    ../dialogs/oscdialog.h \
    ../check/abstractcheckdialog.h \
    ../config/abstractconfdialog.h \
    ../config/confdialog.h \
    ../config/confdialoga1.h \
    ../config/config.h \
    ../config/configa1.h \
    ../check/checkdialoga1.h \
    ../tune/tunedialoga1.h \
    ../dialogs/keypressdialog.h \
    ../check/checka1.h \
    ../dialogs/settingsdialog.h \
    ../dialogs/downloaddialog.h \
    ../tune/abstracttunedialog.h \
    ../tune/tunedialoga1dn.h \
    ../dialogs/a1dialog.h \
    ../report.h \
    ../mainwindow.h \
    ../dialogs/infodialog.h \
    ../widgets/lineeditfield.h

RESOURCES += \
    pkdn-s.qrc

win32 {

    ## Windows common build here

    !contains(QMAKE_TARGET.arch, x86_64) {
        ## Windows x86 (32bit) specific build here
        CONFIG(release, debug|release): LIBS += -L$$PWD/../../LimeReport-master/win32/release/ -llimereport
        CONFIG(debug, debug|release): LIBS += -L$$PWD/../../LimeReport-master/win32/debug/ -llimereport
    } else {
        ## Windows x64 (64bit) specific build here
        CONFIG(release, debug|release): LIBS += -L$$PWD/../../LimeReport-master/win64/release/ -llimereport
        CONFIG(debug, debug|release): LIBS += -L$$PWD/../../LimeReport-master/win64/debug/ -llimereport
    }
}
unix: LIBS += -L$$PWD/LimeReport-master/build/5.5.1/win32/debug/lib/ -llimereport
