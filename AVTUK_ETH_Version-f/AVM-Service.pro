#-------------------------------------------------
#
# Project created by QtCreator 2019-06-13T12:18:20
#
#-------------------------------------------------

QMAKE_TARGET_COMPANY = EvelSoft
QMAKE_TARGET_COPYRIGHT = EvelSoft
QMAKE_TARGET_PRODUCT = AVTUK-S
RC_ICONS = ../coma.ico
CONFIG += c++17
VERSION = 1.1

QT       += core gui printsupport network serialport qml widgets testlib concurrent

TARGET = AVM-Service
DEFINES += PROGNAME='\\"AVM-Service\\"'
DEFINES += PROGCAPTION='\\"AVM-Service\\040v\\040"$$VERSION"\\040\\"'
DEFINES += COMAVERSION='\\"$$VERSION\\"'
DEFINES += DEVICETYPE=1 # 1 - module, 2 - pribor, for diagnostic messages
DEFINES += SOFTDEVELOPER='\\"EvelSoft\\"'
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# DEFINES += DEBUG
TEMPLATE = app


SOURCES += \
../check/checkkdvdialog.cpp \
../check/checkkdvharmonicdialog.cpp \
../check/checkkdvvibrdialog.cpp \
../check/checkktfdialog.cpp \
../check/checkktfharmonicdialog.cpp \
../config/confkdvdialog.cpp \
../config/confktfdialog.cpp \
../dialogs/fwuploaddialog.cpp \
../gen/udialog.cpp \
../module/alarm.cpp \
../module/warn.cpp \
../startup/startupkdvdialog.cpp \
coma.cpp \
main.cpp \
../alarm/alarmclass.cpp \
../alarm/alarmstateall.cpp \
../alarm/alarmwidget.cpp \
../check/abstractcheckdialog.cpp \
../check/check.cpp \
../check/checkharmonickdv.cpp \
../check/checkharmonicktf.cpp \
../check/checkkdv.cpp \
../check/checkkiv.cpp \
../check/checkkivdialog.cpp \
../check/checkktf.cpp \
../check/checkvibrkdv.cpp \
../config/abstractconfdialog.cpp \
../config/config.cpp \
../config/confdialog.cpp \
../config/configkdv.cpp \
../config/configkiv.cpp \
../config/configktf.cpp \
../config/configkxx.cpp \
../config/confkivdialog.cpp \
../config/confkxxdialog.cpp \
../dialogs/connectdialog.cpp \
../dialogs/errordialog.cpp \
../dialogs/infodialog.cpp \
../dialogs/journalsdialog.cpp \
../dialogs/keypressdialog.cpp \
../dialogs/settingsdialog.cpp \
../dialogs/timedialog.cpp \
../gen/board.cpp \
../gen/error.cpp \
../gen/files.cpp \
../gen/journals.cpp \
../gen/logclass.cpp \
../gen/modulebsi.cpp \
../gen/s2.cpp \
../gen/stdfunc.cpp \
../gen/timefunc.cpp \
../iec104/ethernet.cpp \
../iec104/iec104.cpp \
../modbus/modbus.cpp \
../modbus/serialport.cpp \
../models/checkdelegate.cpp \
../models/etableitem.cpp \
../models/etablemodel.cpp \
../models/valuemodel.cpp \
../module/alarmkiv.cpp \
../module/alarmktf.cpp \
../module/module.cpp \
../module/warnkiv.cpp \
../module/warnktf.cpp \
../startup/abstractstartupdialog.cpp \
../startup/startupkivdialog.cpp \
../startup/startupktfdialog.cpp \
../usb/commands.cpp \
../usb/eprotocom.cpp \
../usb/eusbworker.cpp \
../widgets/ecombobox.cpp \
../widgets/edoublespinbox.cpp \
../widgets/errorprotocolwidget.cpp \
../widgets/estackedwidget.cpp \
../widgets/etableview.cpp \
../widgets/etabwidget.cpp \
../widgets/lineeditfield.cpp \
../widgets/passwordlineedit.cpp \
../widgets/signalchoosewidget.cpp \
../widgets/waitwidget.cpp \
../widgets/wd_func.cpp

HEADERS += \
../check/checkkdvdialog.h \
../check/checkkdvharmonicdialog.h \
../check/checkkdvvibrdialog.h \
../check/checkktfdialog.h \
../check/checkktfharmonicdialog.h \
../config/confkdvdialog.h \
../config/confktfdialog.h \
../dialogs/fwuploaddialog.h \
../gen/udialog.h \
../module/alarm.h \
../module/warn.h \
../startup/startupkdvdialog.h \
coma.h \
../alarm/alarmclass.h \
../alarm/alarmstateall.h \
../alarm/alarmwidget.h \
../check/check.h \
../check/abstractcheckdialog.h \
../check/checkharmonickdv.h \
../check/checkharmonicktf.h \
../check/checkkdv.h \
../check/checkkiv.h \
../check/checkkivdialog.h \
../check/checkktf.h \
../check/checkvibrkdv.h \
../config/abstractconfdialog.h \
../config/config.h \
../config/confdialog.h \
../config/configkdv.h \
../config/configkiv.h \
../config/configktf.h \
../config/configkxx.h \
../config/confkivdialog.h \
../config/confkxxdialog.h \
../dialogs/connectdialog.h \
../dialogs/errordialog.h \
../dialogs/infodialog.h \
../dialogs/journalsdialog.h \
../dialogs/keypressdialog.h \
../dialogs/settingsdialog.h \
../dialogs/timedialog.h \
../gen/board.h \
../gen/colors.h \
../gen/error.h \
../gen/files.h \
../gen/journals.h \
../gen/logclass.h \
../gen/modulebsi.h \
../gen/s2.h \
../gen/stdfunc.h \
../gen/timefunc.h \
../iec104/ethernet.h \
../iec104/iec104.h \
../modbus/modbus.h \
../modbus/serialport.h \
../models/checkdelegate.h \
../models/etableitem.h \
../models/etablemodel.h \
../models/valuemodel.h \
../module/alarmkiv.h \
../module/alarmktf.h \
../module/module.h \
../module/warnkiv.h \
../module/warnktf.h \
../startup/abstractstartupdialog.h \
../startup/startupkivdialog.h \
../startup/startupktfdialog.h \
../usb/commands.h \
../usb/defines.h \
../usb/eprotocom.h \
../usb/eusbworker.h \
../widgets/ecombobox.h \
../widgets/edoublespinbox.h \
../widgets/errorprotocolwidget.h \
../widgets/estackedwidget.h \
../widgets/etableview.h \
../widgets/etabwidget.h \
../widgets/keypress.h \
../widgets/lineeditfield.h \
../widgets/passwordlineedit.h \
../widgets/signalchoosewidget.h \
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
       LIBS += -L$$PWD/../../libs/win64/debug/  -lliblzma -lhidapi
       DESTDIR = $${PWD}/../../build/win64/debug
       } else {
       LIBS += -L$$PWD/../../libs/win64/release/ -lliblzma -lhidapi
       DESTDIR = $${PWD}/../../build/win64/release
       LIBS_FILES += \
       $$PWD/../../libs/win64/release/hidapi.dll \
       $$PWD/../../libs/win64/release/liblzma.dll
       }
    } else {
        message("Windows x86 build")
        ## Windows x86 (32bit) specific build here
        CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../../libs/win32/debug/  -lliblzma -lhidapi
        DESTDIR = $${PWD}/../../build/win32/debug
        } else {
        LIBS += -L$$PWD/../../libs/win32/release/  -lliblzma -lhidapi
        DESTDIR = $${PWD}/../../build/win32/release
        LIBS_FILES += \
        $$PWD/../../libs/win32/release/hidapi.dll \
        $$PWD/../../libs/win32/release/liblzma.dll
        }
    }
}

unix {
LIBS += -llzma
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

unix: LIBS += -lhidapi-hidraw
