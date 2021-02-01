include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets xml printsupport qml

TARGET = Module$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += MODULE_LIBRARY

# QXlsx code for Application Qt project
QXLSX_PARENTPATH=./../../include/QXlsx/QXlsx         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./../../include/QXlsx/QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./../../include/QXlsx/QXlsx/source/  # current QXlsx source path is ./source/
include(./../../include/QXlsx/QXlsx/QXlsx.pri)

INCLUDEPATH += ./../../include/LimeReport

SOURCES += \
    alarmstateall.cpp \
    basealarm.cpp \
    journals.cpp \
    journkdv.cpp \
    journkiv.cpp \
    journktf.cpp \
    module.cpp \
    modulealarm.cpp \

HEADERS += \
    alarmstateall.h \
    basealarm.h \
    journals.h \
    journkdv.h \
    journkiv.h \
    journktf.h \
    module.h \
    module_kiv.h \
    module_kxx.h \
    modulealarm.h \
    modules.h \
    registers.h \
