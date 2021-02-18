include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets xml svg printsupport qml

TARGET = tune$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += TUNE_LIBRARY

INCLUDEPATH += ../$${QXLSX_PARENTPATH}
INCLUDEPATH += ../$${QXLSX_HEADERPATH}
INCLUDEPATH += ../$${LIME_HEADERPATH}

include(tune.pri)
