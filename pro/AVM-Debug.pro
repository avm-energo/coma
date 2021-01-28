#-------------------------------------------------
#
# Project created by QtCreator 2019-06-13T12:18:20
#
#-------------------------------------------------

QMAKE_TARGET_COMPANY = AVM-Energo
QMAKE_TARGET_COPYRIGHT = AVM-Energo
QMAKE_TARGET_PRODUCT = AVTUK
RC_ICONS = ../coma.ico
CONFIG += c++17
CONFIG -= console
VERSION = 2.0.0

QT       += core gui printsupport network qml serialport widgets testlib concurrent xml svg
QT.testlib.CONFIG -= console

TARGET = AVM-Debug
DEFINES += PROGNAME='\\"AVM-Debug\\"'
DEFINES += PROGCAPTION='\\"AVM-Debug\\040v\\040"$$VERSION"\\040\\"'
DEFINES += COMAVERSION='\\"$$VERSION\\"'
DEFINES += DEVICETYPE=1 # 1 - module, 2 - pribor, for diagnostic messages
DEFINES += SOFTDEVELOPER='\\"$$QMAKE_TARGET_COMPANY\\"'
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
#// NOTE Реализовать
#DEFINES += GIT_VERSION='$(shell git describe --always)'
#DEFINES += BUILD_DATE='"\\\"$(shell date)\\\""'

DEFINES += AVM_DEBUG
TEMPLATE = app


SOURCES += \
    ../datablocks/bd0.cpp \
    ../datablocks/datablock.cpp \
    ../datablocks/kiv/bac.cpp \
    ../datablocks/kiv/bda.cpp \
    ../datablocks/kiv/bdain.cpp \
    ../gen/errorqueue.cpp \
    ../gen/helper.cpp \
    ../interfaces/interfacesettings.cpp \
    ../interfaces/protocom_p.cpp \
    ../interfaces/usbhidsettings.cpp \
    ../models/errorprotocolmodel.cpp \
    ../module/alarmkdv.cpp \
    ../module/journkdv.cpp \
    ../module/journkiv.cpp \
    ../module/journktf.cpp \
    ../tune/generaltunedialog.cpp \
    ../tune/kiv/tunekivadc.cpp \
    ../tune/kiv/tunekivr.cpp \
    ../widgets/aboutwidget.cpp \
    ../widgets/edoublespinbox.cpp \
    ../widgets/styleloader.cpp \
    coma.cpp \
    main.cpp \
    ../check/abstractcheckdialog.cpp \
    ../check/check.cpp \
    ../check/checkharmonickdv.cpp \
    ../check/checkharmonicktf.cpp \
    ../check/checkkdv.cpp \
    ../check/checkkdvdialog.cpp \
    ../check/checkkdvharmonicdialog.cpp \
    ../check/checkkdvvibrdialog.cpp \
    ../check/checkkiv.cpp \
    ../check/checkkivdialog.cpp \
    ../check/checkktf.cpp \
    ../check/checkktfdialog.cpp \
    ../check/checkktfharmonicdialog.cpp \
    ../check/checkvibrkdv.cpp \
    ../config/abstractconfdialog.cpp \
    ../config/config.cpp \
    ../config/configkdv.cpp \
    ../config/configkiv.cpp \
    ../config/configktf.cpp \
    ../config/configkxx.cpp \
    ../config/confkivdialog.cpp \
    ../config/confkdvdialog.cpp \
    ../config/confktfdialog.cpp \
    ../dialogs/connectdialog.cpp \
    ../dialogs/errordialog.cpp \
    ../dialogs/infodialog.cpp \
    ../dialogs/fwuploaddialog.cpp \
    ../dialogs/journalsdialog.cpp \
    ../dialogs/keypressdialog.cpp \
    ../dialogs/settingsdialog.cpp \
    ../dialogs/timedialog.cpp \
    ../gen/board.cpp \
    ../gen/error.cpp \
    ../gen/files.cpp \
    ../gen/datamanager.cpp \
    ../gen/logclass.cpp \
    ../gen/logger.cpp \
    ../gen/report.cpp \
    ../gen/s2.cpp \
    ../gen/stdfunc.cpp \
    ../gen/timefunc.cpp \
    ../gen/pch.cpp \
    ../interfaces/baseinterface.cpp \
    ../interfaces/protocom.cpp \
    ../interfaces/protocomthread.cpp \
    ../interfaces/usbhidport.cpp\
    ../interfaces/usbhidportinfo.cpp \
    ../models/etableitem.cpp \
    ../models/etablemodel.cpp \
    ../module/alarmkiv.cpp \
    ../module/alarmktf.cpp \
    ../module/journals.cpp \
    ../module/module.cpp \
    ../module/basealarm.cpp \
    ../module/modulealarm.cpp \
    ../startup/abstractstartupdialog.cpp \
    ../startup/startupkdvdialog.cpp \
    ../startup/startupkivdialog.cpp \
    ../startup/startupktfdialog.cpp \
    ../tune/abstracttunedialog.cpp \
    ../tune/kiv/tunekiv.cpp \
    ../tune/kiv/tunekivcheck.cpp \
    ../tune/kiv/tunekivdialog.cpp \
    ../tune/kiv/tunekivtemp60.cpp \
    ../widgets/alarmwidget.cpp \
    ../widgets/alarmbutton.cpp \
    ../widgets/alarmstateall.cpp \
    ../widgets/ecombobox.cpp \
    ../widgets/errorprotocolwidget.cpp \
    ../widgets/estackedwidget.cpp \
    ../widgets/etableview.cpp \
    ../widgets/etabwidget.cpp \
    ../widgets/lineeditfield.cpp \
    ../widgets/passwordlineedit.cpp \
    ../widgets/signalchoosewidget.cpp \
    ../widgets/splashscreen.cpp \
    ../widgets/udialog.cpp \
    ../widgets/uwidget.cpp \
    ../widgets/waitwidget.cpp \
    ../widgets/wd_func.cpp

#PRECOMPILED_HEADER = ../gen/pch.h

HEADERS += \
    ../datablocks/bd0.h \
    ../datablocks/datablock.h \
    ../datablocks/kiv/bac.h \
    ../datablocks/kiv/bda.h \
    ../datablocks/kiv/bdain.h \
    ../gen/errorqueue.h \
    ../gen/helper.h \
    ../interfaces/interfacesettings.h \
    ../interfaces/protocom_p.h \
    ../interfaces/usbhidsettings.h \
    ../models/errorprotocolmodel.h \
    ../module/alarmkdv.h \
    ../module/journkdv.h \
    ../module/journkiv.h \
    ../module/journktf.h \
    ../tune/generaltunedialog.h \
    ../tune/kiv/tunekivadc.h \
    ../tune/kiv/tunekivr.h \
    ../tune/tunesteps.h \
    ../widgets/aboutwidget.h \
    ../widgets/edoublespinbox.h \
    ../widgets/styleloader.h \
    coma.h \
    ../check/abstractcheckdialog.h \
    ../check/check.h \
    ../check/checkharmonickdv.h \
    ../check/checkharmonicktf.h \
    ../check/checkkdv.h \
    ../check/checkkdvdialog.h \
    ../check/checkkdvharmonicdialog.h \
    ../check/checkkdvvibrdialog.h \
    ../check/checkkiv.h \
    ../check/checkkivdialog.h \
    ../check/checkktf.h \
    ../check/checkktfdialog.h \
    ../check/checkktfharmonicdialog.h \
    ../check/checkvibrkdv.h \
    ../config/confkdvdialog.h \
    ../config/confktfdialog.h \
    ../config/abstractconfdialog.h \
    ../config/config.h \
    ../config/configkdv.h \
    ../config/configkiv.h \
    ../config/configktf.h \
    ../config/configkxx.h \
    ../config/confkivdialog.h \
    ../dialogs/connectdialog.h \
    ../dialogs/errordialog.h \
    ../dialogs/infodialog.h \
    ../dialogs/fwuploaddialog.h \
    ../dialogs/journalsdialog.h \
    ../dialogs/keypressdialog.h \
    ../dialogs/settingsdialog.h \
    ../dialogs/timedialog.h \
    ../gen/board.h \
	../gen/pch.h \
    ../gen/colors.h \
    ../gen/datatypes.h \
    ../gen/datamanager.h \
    ../gen/error.h \
    ../gen/files.h \
    ../gen/logclass.h \
    ../gen/logger.h \
    ../gen/report.h \
    ../gen/s2.h \
    ../gen/stdfunc.h \
    ../gen/timefunc.h \
    ../interfaces/baseinterface.h \
    ../interfaces/protocomprivate.h \
    ../interfaces/protocom.h \
    ../interfaces/protocomthread.h \
    ../interfaces/usbhidport.h \
    ../interfaces/usbhidportinfo.h \
    ../models/etableitem.h \
    ../models/etablemodel.h \
    ../module/alarmkiv.h \
    ../module/alarmktf.h \
    ../module/journals.h \
    ../module/module.h \
    ../module/modules.h \
    ../module/module_kxx.h \
    ../module/module_kiv.h \
    ../module/registers.h \
    ../module/basealarm.h \
    ../module/modulealarm.h \
    ../startup/abstractstartupdialog.h \
    ../startup/startupkdvdialog.h \
    ../startup/startupkivdialog.h \
    ../startup/startupktfdialog.h \
    ../tune/abstracttunedialog.h \
    ../tune/kiv/tunekivcheck.h \
    ../tune/kiv/tunekiv.h \
    ../tune/kiv/tunekivdialog.h \
    ../tune/kiv/tunekivtemp60.h \
    ../widgets/alarmbutton.h \
    ../widgets/alarmstateall.h \
    ../widgets/alarmwidget.h \
    ../widgets/ecombobox.h \
    ../widgets/errorprotocolwidget.h \
    ../widgets/estackedwidget.h \
    ../widgets/etableview.h \
    ../widgets/etabwidget.h \
    ../widgets/keypress.h \
    ../widgets/lineeditfield.h \
    ../widgets/passwordlineedit.h \
    ../widgets/signalchoosewidget.h \
    ../widgets/splashscreen.h \
    ../widgets/udialog.h \
    ../widgets/uwidget.h \
    ../widgets/waitwidget.h \
    ../widgets/wd_func.h


IMAGE_FILE += \
    $$PWD/../images

ERROR_FILES += \
    $$PWD/../errors

RESOURCES += \
    ../widgets/QDarkStyleSheet/qdarkstyle/style.qrc \
    ../images/vectorIcons.qrc \
    ../settings/settings.qrc \
    ../widgets/styles.qrc

INCLUDEPATH += $$PWD/../include

# QXlsx code for Application Qt project
QXLSX_PARENTPATH=./../QXlsx/QXlsx         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./../QXlsx/QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./../QXlsx/QXlsx/source/  # current QXlsx source path is ./source/
include(./../QXlsx/QXlsx/QXlsx.pri)

include(common.pri)

win32 {
    LIBS += -luser32
    contains(QMAKE_TARGET.arch, x86_64) {
        message("x64 build")
       ## Windows x64 (64bit) specific build here
       CONFIG(debug, debug|release) {
       LIBS += -L$$PWD/../lib/win64/debug/ -llimereportd -lhidapi
       DESTDIR = $${PWD}/../bin/$${TARGET}/win64/debug
       } else {
       LIBS += -L$$PWD/../lib/win64/release/ -llimereport -lhidapi
       DESTDIR = $${PWD}/../bin/$${TARGET}/win64/release
       LIBS_FILES += \
       $$PWD/../lib/win64/release/hidapi.dll \
       $$PWD/../lib/win64/release/limereport.dll \
       $$PWD/../lib/win64/release/QtZint.dll
       }
    } else {
        message("x86 build")
        ## Windows x86 (32bit) specific build here
        CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../lib/win32/debug/ -llimereportd -lhidapi
        DESTDIR = $${PWD}/../bin/$${TARGET}/win32/debug
        } else {
        LIBS += -L$$PWD/../lib/win32/release/ -llimereport -lhidapi
        DESTDIR = $${PWD}/../bin/$${TARGET}/win32/release
        LIBS_FILES += \
        $$PWD/../lib/win32/release/hidapi.dll \
        $$PWD/../lib/win32/release/limereport.dll \
        $$PWD/../lib/win32/release/QtZint.dll
        }
    }
}

unix: LIBS += -llimereport

copyToDestDir($$IMAGE_FILE, $$DESTDIR/images/)
copyToDestDir($$ERROR_FILES, $$DESTDIR/errors/)
copyToDestDir($$LIBS_FILES, $$DESTDIR/)