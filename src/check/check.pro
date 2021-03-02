include( ../common.pri )
include( ../lib.pri )
QT += core gui widgets

TARGET = check$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += CHECK_LIBRARY


INCLUDEPATH += ../$${QXLSX_PARENTPATH}
INCLUDEPATH += ../$${QXLSX_HEADERPATH}

SOURCES += \
    check.cpp \
    abstractcheckdialog.cpp \
    checkkivdialog.cpp \
    checkkdvdialog.cpp \
    checkktfdialog.cpp \
    checkkdvharmonicdialog.cpp \
    checkkdvvibrdialog.cpp \
    checkktfharmonicdialog.cpp \

HEADERS += \
    check.h \
    abstractcheckdialog.h \
    checkkivdialog.h \
    checkkdvdialog.h \
    checkktfdialog.h \
    checkktfharmonicdialog.h \
    checkkdvharmonicdialog.h  \
    checkkdvvibrdialog.h \











