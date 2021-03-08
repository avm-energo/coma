include(common.pri)

#include(gen\gen.pri)
QT       += core gui printsupport network serialport widgets concurrent testlib xml qml
QT.testlib.CONFIG -= console
TARGET = AVM-Service
# Этот проект - это галимый, никому не нужный файл, менять его нет смысла, просто смирись с тем, что разработчику он за каким-то лядом нужен
VERSION = 2.1.2

TEMPLATE = app

RC_ICONS = ../coma.ico

DEFINES -= QT_DISABLE_DEPRECATED_BEFORE=0x060000

QMAKE_SUBSTITUTES += config.h.in

INCLUDEPATH += $${QXLSX_PARENTPATH}
INCLUDEPATH += $${QXLSX_HEADERPATH}
INCLUDEPATH += $${QXLSX_SOURCEPATH}

include(../include/QXlsx/QXlsx/QXlsx.pri)


unix {
LIBS += -lhidapi-hidraw
contains(QT_ARCH, x86_64) {
        message("Unix x64 build")
        ## Unix x64 (64bit) specific build here
        CONFIG(debug, debug|release) {

        }
    } else {
        message("Unix x86 build")
        ## Unix x86 (32bit) specific build here
        CONFIG(debug, debug|release) {

        }
    }
}

include(AVM-Service/AVM-Service.pri)
include(check/check.pri)
include(config/config.pri)
include(dialogs/dialogs.pri)
include(gen/gen.pri)
include(interfaces/interfaces.pri)
include(models/models.pri)
include(module/module.pri)
include(startup/startup.pri)
include(widgets/widgets.pri)

RESOURCES += \
    ../images/vectorIcons.qrc \
    widgets/styles/QDarkStyleSheet/qdarkstyle/style.qrc \
    widgets/styles/styles.qrc
