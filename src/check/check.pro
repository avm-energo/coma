include( ../common.pri )
include( ../lib.pri )
QT += core gui widgets

TARGET = check$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += CHECK_LIBRARY


INCLUDEPATH += ../$${QXLSX_PARENTPATH}
INCLUDEPATH += ../$${QXLSX_HEADERPATH}

include(check.pri)











