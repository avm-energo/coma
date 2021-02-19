include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets

TARGET = avtuk$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += AVTUK_LIBRARY

INCLUDEPATH += ../$${QXLSX_PARENTPATH}
INCLUDEPATH += ../$${QXLSX_HEADERPATH}

include(avtuk.pri)
