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
VERSION = 0.9.1

QT       += core gui printsupport network serialport qml widgets testlib

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
    ../alarm/abstractalarm.cpp \
    ../alarm/alarmclass.cpp \
    ../alarm/alarmstateall.cpp \
    ../alarm/alarmwidget.cpp \
    ../alarm/avaralarmkiv.cpp \
    ../alarm/avaralarmktf.cpp \
    ../alarm/warnalarmkiv.cpp \
    ../alarm/warnalarmktf.cpp \
    ../check/checkdialogharmonickdv.cpp \
    ../check/checkdialogharmonicktf.cpp \
    ../check/checkdialogkdv.cpp \
    ../check/checkdialogkiv.cpp \
    ../check/checkdialogktf.cpp \
    ../check/checkdialogvibrkdv.cpp \
    ../check/checkharmonickdv.cpp \
    ../check/checkharmonicktf.cpp \
    ../check/checkkdv.cpp \
    ../check/checkkiv.cpp \
    ../check/checkktf.cpp \
    ../check/checkvibrkdv.cpp \
    ../config/confdialogkdv.cpp \
    ../config/confdialogkiv.cpp \
    ../config/confdialogktf.cpp \
    ../config/confdialogkxx.cpp \
    ../config/configkdv.cpp \
    ../config/configkiv.cpp \
    ../config/configktf.cpp \
    ../config/configkxx.cpp \
    ../dialogs/abstractcordialog.cpp \
    ../dialogs/connectdialog.cpp \
    ../dialogs/cordialogkdv.cpp \
    ../dialogs/cordialogktf.cpp \
    ../dialogs/infodialog.cpp \
    ../dialogs/mnktime.cpp \
    ../gen/board.cpp \
    ../gen/journals.cpp \
    ../gen/logclass.cpp \
    ../gen/logger.cpp \
    ../modbus/serialport.cpp \
    ../usb/eprotocom.cpp \
    ../usb/eusbworker.cpp \
    ../widgets/edoublespinbox.cpp \
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
    ../models/etableitem.cpp \
    ../models/etablemodel.cpp \
    ../models/getoscpbdelegate.cpp \
    ../modbus/modbus.cpp \
    ../usb/commands.cpp \
    ../widgets/ecombobox.cpp \
    ../widgets/errorprotocolwidget.cpp \
    ../widgets/estackedwidget.cpp \
    ../widgets/etableview.cpp \
    ../widgets/etabwidget.cpp \
    ../widgets/lineeditfield.cpp \
    ../widgets/passwordlineedit.cpp \
    ../widgets/signalchoosewidget.cpp \
    ../widgets/wd_func.cpp

HEADERS += \
    ../alarm/abstractalarm.h \
    ../alarm/alarmclass.h \
    ../alarm/alarmstateall.h \
    ../alarm/alarmwidget.h \
    ../alarm/avaralarmkiv.h \
    ../alarm/avaralarmktf.h \
    ../alarm/warnalarmkiv.h \
    ../alarm/warnalarmktf.h \
    ../check/checkdialogharmonickdv.h \
    ../check/checkdialogharmonicktf.h \
    ../check/checkdialogkdv.h \
    ../check/checkdialogkiv.h \
    ../check/checkdialogktf.h \
    ../check/checkdialogvibrkdv.h \
    ../check/checkharmonickdv.h \
    ../check/checkharmonicktf.h \
    ../check/checkkdv.h \
    ../check/checkkiv.h \
    ../check/checkktf.h \
    ../check/checkvibrkdv.h \
    ../config/abstractconfdialog.h \
    ../config/confdialogkdv.h \
    ../config/confdialogkiv.h \
    ../config/confdialogktf.h \
    ../config/confdialogkxx.h \
    ../config/configkdv.h \
    ../config/configkiv.h \
    ../config/configktf.h \
    ../config/configkxx.h \
    ../dialogs/abstractcordialog.h \
    ../dialogs/connectdialog.h \
    ../dialogs/cordialogkdv.h \
    ../dialogs/cordialogktf.h \
    ../dialogs/infodialog.h \
    ../dialogs/mnktime.h \
    ../gen/board.h \
    ../gen/journals.h \
    ../gen/logclass.h \
    ../gen/logger.h \
    ../modbus/serialport.h \
    ../usb/defines.h \
    ../usb/eprotocom.h \
    ../usb/eusbworker.h \
    ../widgets/edoublespinbox.h \
    coma.h \
    ../check/check.h \
    ../check/eabstractcheckdialog.h \
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
    ../gen/modulebsi.h \
    ../gen/report.h \
    ../gen/s2.h \
    ../gen/stdfunc.h \
    ../gen/timefunc.h \
    ../iec104/ethernet.h \
    ../iec104/iec104.h \
    ../models/etableitem.h \
    ../models/getoscpbdelegate.h \
    ../models/etablemodel.h \
    ../usb/commands.h \
    ../widgets/ecombobox.h \
    ../widgets/errorprotocolwidget.h \
    ../widgets/estackedwidget.h \
    ../widgets/etableview.h \
    ../widgets/etabwidget.h \
    ../widgets/keypress.h \
    ../widgets/lineeditfield.h \
    ../widgets/passwordlineedit.h \
    ../widgets/signalchoosewidget.h \
    ../widgets/wd_func.h \
    ../modbus/modbus.h


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
       LIBS += -L$$PWD/../../libs/win64/debug/ -llimereportd -lliblzma -lhidapi
       DESTDIR = $${PWD}/../../build/win64/debug
       } else {
       LIBS += -L$$PWD/../../libs/win64/release/ -llimereport -lliblzma -lhidapi
       DESTDIR = $${PWD}/../../build/win64/release
       LIBS_FILES += \
       $$PWD/../../libs/win64/release/hidapi.dll \
       $$PWD/../../libs/win64/release/liblzma.dll \
       $$PWD/../../libs/win64/release/QtZint.dll \
       $$PWD/../../libs/win64/release/limereport.dll
       }
    } else {
        message("Windows x86 build")
        ## Windows x86 (32bit) specific build here
        CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/../../libs/win32/debug/ -llimereportd -lliblzma -lhidapi
        DESTDIR = $${PWD}/../../build/win32/debug
        } else {
        LIBS += -L$$PWD/../../libs/win32/release/ -llimereport -lliblzma -lhidapi
        DESTDIR = $${PWD}/../../build/win32/release
        LIBS_FILES += \
        $$PWD/../../libs/win32/release/hidapi.dll \
        $$PWD/../../libs/win32/release/liblzma.dll \
        $$PWD/../../libs/win32/release/QtZint.dll \
        $$PWD/../../libs/win32/release/limereport.dll
        }
    }
}

unix {
LIBS += -lhidapi-libusb -llzma
contains(QT_ARCH, x86_64) {
        message("Unix x64 build")
        ## Unix x64 (64bit) specific build here
        LIBS += -L$$PWD/../LimeReport/build/$${QT_VERSION}/linux64/release/lib  -llimereport -lQtZint -lhidapi-libusb -llzma
        INCLUDEPATH += $$PWD/../LimeReport/build/$${QT_VERSION}/linux64/release/lib/include
        DEPENDPATH += $$PWD/../LimeReport/build/$${QT_VERSION}/linux64/release
        CONFIG(debug, debug|release) {
        DESTDIR = $${PWD}/../../build/linux64/debug
        } else {
        DESTDIR = $${PWD}/../../build/linux64/release
        }
    } else {
        message("Unix x86 build")
        ## Unix x86 (32bit) specific build here
        LIBS += -L$$PWD/../LimeReport/build/$${QT_VERSION}/linux32/release/lib  -llimereport -lQtZint -lhidapi-libusb -llzma
        INCLUDEPATH += $$PWD/../LimeReport/build/$${QT_VERSION}/linux32/release/lib/include
        DEPENDPATH += $$PWD/../LimeReport/build/$${QT_VERSION}/linux32/release
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
