include( ../common.pri )
include( ../lib.pri )

QT += core concurrent gui widgets xml svg printsupport qml

TARGET = tune$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += TUNE_LIBRARY

INCLUDEPATH += ../$${QXLSX_PARENTPATH}
INCLUDEPATH += ../$${QXLSX_HEADERPATH}
INCLUDEPATH += ../$${QXLSX_SOURCEPATH}
INCLUDEPATH += ../$${LIME_HEADERPATH}

SOURCES += \
    abstracttunedialog.cpp \
    generaltunedialog.cpp \



HEADERS += \
    abstracttunedialog.h \
    generaltunedialog.h \
    tunesteps.h

include(tunekiv.pri)
