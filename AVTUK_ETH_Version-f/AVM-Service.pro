#-------------------------------------------------
#
# Project created by QtCreator 2019-06-13T12:18:20
#
#-------------------------------------------------

QMAKE_TARGET_COMPANY = AVM-Energo
QMAKE_TARGET_COPYRIGHT = AVM-Energo
QMAKE_TARGET_PRODUCT = AVTUK-S
RC_ICONS = ../coma.ico
CONFIG += c++17
CONFIG -= console
VERSION = 1.1.2

QT       += core gui printsupport network serialport widgets concurrent testlib xml
QT.testlib.CONFIG -= console
TARGET = AVM-Service
DEFINES += PROGNAME='\\"AVM-Service\\"'
DEFINES += COMAVERSION='\\"$$VERSION\\"'
DEFINES += DEVICETYPE=1 # 1 - module, 2 - pribor, for diagnostic messages
DEFINES += SOFTDEVELOPER='\\"$$QMAKE_TARGET_COMPANY\\"'
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
#// NOTE Реализовать
#DEFINES += GIT_VERSION='$(shell git describe --always)'
#DEFINES += GIT_TAG="\\\"$(shell git describe --tags)\\\""
#DEFINES += GIT_CURRENT_SHA1="\\\"$(shell git -C \""$$_PRO_FILE_PWD_"\" describe)\\\""
# DEFINES += DEBUG
TEMPLATE = app


SOURCES += \
    ../interfaces/protocom_p.cpp \
    ../interfaces/usbhidsettings.cpp \
    ../widgets/aboutwidget.cpp \
    ../widgets/alarmbutton.cpp \
    ../widgets/alarmstateall.cpp \
    ../widgets/styleloader.cpp \
    coma.cpp \
    main.cpp \
    ../check/abstractcheckdialog.cpp \
    ../check/check.cpp \
    ../check/checkharmonickdv.cpp \
    ../check/checkharmonicktf.cpp \
    ../check/checkkdv.cpp \
    ../check/checkkiv.cpp \
    ../check/checkkivdialog.cpp \
    ../check/checkktf.cpp \
    ../check/checkvibrkdv.cpp \
    ../check/checkkdvdialog.cpp \
    ../check/checkkdvharmonicdialog.cpp \
    ../check/checkkdvvibrdialog.cpp \
    ../check/checkktfdialog.cpp \
    ../check/checkktfharmonicdialog.cpp \
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
    ../dialogs/fwuploaddialog.cpp \
    ../dialogs/infodialog.cpp \
    ../dialogs/journalsdialog.cpp \
    ../dialogs/keypressdialog.cpp \
    ../dialogs/settingsdialog.cpp \
    ../dialogs/timedialog.cpp \
    ../gen/board.cpp \
    ../gen/error.cpp \
    ../gen/datamanager.cpp \
    ../gen/errorqueue.cpp \
    ../gen/files.cpp \
    ../gen/logclass.cpp \
    ../gen/logger.cpp \
    ../gen/helper.cpp \
    ../gen/s2.cpp \
    ../gen/stdfunc.cpp \
    ../gen/timefunc.cpp \
    ../gen/pch.cpp \
    ../interfaces/baseinterface.cpp \
    ../interfaces/ethernet.cpp \
    ../interfaces/iec104.cpp \
    ../interfaces/iec104thread.cpp \
    ../interfaces/modbus.cpp \
    ../interfaces/modbusthread.cpp \
    ../interfaces/serialport.cpp \
    ../interfaces/usbhidport.cpp \
    ../interfaces/protocom.cpp \
    ../interfaces/protocomthread.cpp \
    ../interfaces/usbhidportinfo.cpp \
    ../models/errorprotocolmodel.cpp \
    ../models/etableitem.cpp \
    ../models/etablemodel.cpp \
    ../module/basealarm.cpp \
    ../module/modulealarm.cpp \
    ../module/journals.cpp \
    ../module/journkdv.cpp \
    ../module/journkiv.cpp \
    ../module/journktf.cpp \
    ../module/module.cpp \
    ../startup/abstractstartupdialog.cpp \
    ../startup/startupkdvdialog.cpp \
    ../startup/startupkivdialog.cpp \
    ../startup/startupktfdialog.cpp \
    ../widgets/alarmwidget.cpp \
    ../widgets/edoublespinbox.cpp \
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

PRECOMPILED_HEADER = ../gen/pch.h

HEADERS += \
    ../interfaces/protocom_p.h \
    ../interfaces/protocomsettings.h \
    ../interfaces/usbhidsettings.h \
    ../widgets/aboutwidget.h \
    ../widgets/alarmbutton.h \
    ../widgets/alarmstateall.h \
    ../widgets/styleloader.h \
    coma.h \
    ../check/check.h \
    ../check/abstractcheckdialog.h \
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
    ../config/abstractconfdialog.h \
    ../config/config.h \
    ../config/configkdv.h \
    ../config/configkiv.h \
    ../config/configktf.h \
    ../config/configkxx.h \
    ../config/confkivdialog.h \
    ../config/confkdvdialog.h \
    ../config/confktfdialog.h \
    ../dialogs/connectdialog.h \
    ../dialogs/errordialog.h \
    ../dialogs/fwuploaddialog.h \
    ../dialogs/infodialog.h \
    ../dialogs/journalsdialog.h \
    ../dialogs/keypressdialog.h \
    ../dialogs/settingsdialog.h \
    ../dialogs/timedialog.h \
    ../gen/board.h \
    ../gen/colors.h \
    ../gen/datamanager.h \
    ../gen/datatypes.h \
    ../gen/error.h \
    ../gen/errorqueue.h \
    ../gen/files.h \
    ../gen/logger.h \
    ../gen/logclass.h \
    ../gen/helper.h \
    ../gen/s2.h \
    ../gen/stdfunc.h \
    ../gen/timefunc.h \
    ../interfaces/baseinterface.h \
    ../interfaces/ethernet.h \
    ../interfaces/iec104.h \
    ../interfaces/iec104thread.h \
    ../interfaces/modbus.h \
    ../interfaces/modbusthread.h \
    ../interfaces/serialport.h \
    ../interfaces/protocomthread.h \
    ../interfaces/usbhidport.h \
    ../interfaces/protocom.h \
    ../interfaces/settingstypes.h \
    ../interfaces/iec104private.h \
    ../interfaces/modbusprivate.h \
    ../interfaces/usbhidportinfo.h \
    ../interfaces/protocomprivate.h \
    ../models/errorprotocolmodel.h \
    ../models/etableitem.h \
    ../models/etablemodel.h \
    ../module/modulealarm.h \
    ../module/basealarm.h \
    ../module/journals.h \
    ../module/journkdv.h \
    ../module/journkiv.h \
    ../module/journktf.h \
    ../module/module.h \
    ../module/modules.h \
    ../module/module_kiv.h \
    ../module/module_kxx.h \
    ../module/registers.h \
    ../startup/abstractstartupdialog.h \
    ../startup/startupkdvdialog.h \
    ../startup/startupkivdialog.h \
    ../startup/startupktfdialog.h \
    ../widgets/alarmwidget.h \
    ../widgets/edoublespinbox.h \
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



INCLUDEPATH += $$PWD/../../includes

# QXlsx code for Application Qt project
QXLSX_PARENTPATH=./../QXlsx/QXlsx         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./../QXlsx/QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./../QXlsx/QXlsx/source/  # current QXlsx source path is ./source/
include(./../QXlsx/QXlsx/QXlsx.pri)


win32 {
    LIBS += -luser32
    contains(QMAKE_TARGET.arch, x86_64) {
       message("Windows x64 build")
       ## Windows x64 (64bit) specific build here
       CONFIG(debug, debug|release) {
       LIBS += -L$$PWD/../../libs/win64/debug/ -lhidapi
       DESTDIR = $${PWD}/../../build/win64/debug
       } else {
       LIBS += -L$$PWD/../../libs/win64/release/ -lhidapi
       DESTDIR = $${PWD}/../../build/win64/release
       LIBS_FILES += \
       $$PWD/../../libs/win64/release/hidapi.dll
       }
    } else {
        message("Windows x86 build")
        ## Windows x86 (32bit) specific build here
        CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../../libs/win32/debug/ -lhidapi
        DESTDIR = $${PWD}/../../build/win32/debug
        } else {
        LIBS += -L$$PWD/../../libs/win32/release/ -lhidapi
        DESTDIR = $${PWD}/../../build/win32/release
        LIBS_FILES += \
        $$PWD/../../libs/win32/release/hidapi.dll
        }
    }
}

unix {
LIBS += -lhidapi-hidraw
contains(QT_ARCH, x86_64) {
        message("Unix x64 build")
        ## Unix x64 (64bit) specific build here
        CONFIG(debug, debug|release) {
        DESTDIR = $${PWD}/../../build/linux64/debug
        } else {
        DESTDIR = $${PWD}/../../build/linux64/release
        }
    } else {
        message("Unix x86 build")
        ## Unix x86 (32bit) specific build here
        CONFIG(debug, debug|release) {
        DESTDIR = $${PWD}/../../build/linux32/debug
        } else {
        DESTDIR = $${PWD}/../../build/linux32/release
        }
    }
}

# copies the given files to the destination directory
defineTest(copyToDestDir) {
    files = $$1
    dir = $$2
    # replace slashes in destination path for Windows
    win32:dir ~= s,/,\\,g

    for(file, files) {
        # replace slashes in source path for Windows
        win32:file ~= s,/,\\,g

        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$shell_quote($$file) $$shell_quote($$dir) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

copyToDestDir($$IMAGE_FILE, $$DESTDIR/images/)
copyToDestDir($$ERROR_FILES, $$DESTDIR/errors/)
copyToDestDir($$LIBS_FILES, $$DESTDIR/)

DISTFILES += \
    ../interfaces/interfaces.qmodel

RESOURCES += \
    ../QDarkStyleSheet/qdarkstyle/style.qrc \
    ../images/vectorIcons.qrc \
    ../widgets/styles.qrc
