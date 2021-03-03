include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets xml printsupport qml

TARGET = module$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += MODULE_LIBRARY

# QXlsx code for Application Qt project
QXLSX_PARENTPATH=./../../include/QXlsx/QXlsx         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./../../include/QXlsx/QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./../../include/QXlsx/QXlsx/source/  # current QXlsx source path is ./source/
include(./../../include/QXlsx/QXlsx/QXlsx.pri)

INCLUDEPATH += ./../../include/LimeReport

include(module.pri)

RESOURCES += \
    ../../settings/settings.qrc
