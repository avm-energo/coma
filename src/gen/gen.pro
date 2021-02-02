include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets serialport network concurrent printsupport qml

TARGET = general$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += GENERAL_LIBRARY

INCLUDEPATH += ../$${LIME_HEADERPATH}

include(gen.pri)



