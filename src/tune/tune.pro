include( ../common.pri )
include( ../lib.pri )

QT += core concurrent gui widgets xml svg printsupport qml

TARGET = Tune$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += TUNE_LIBRARY

INCLUDEPATH += ./../../include/LimeReport

SOURCES += \
    abstracttunedialog.cpp \
    generaltunedialog.cpp \



HEADERS += \
    abstracttunedialog.h \
    generaltunedialog.h \
    tunesteps.h


