include( ../common.pri )
include( ../lib.pri )
QT += core gui widgets

TARGET = check$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += CHECK_LIBRARY


INCLUDEPATH += ../$${QXLSX_PARENTPATH}
INCLUDEPATH += ../$${QXLSX_HEADERPATH}
INCLUDEPATH += ../$${QXLSX_SOURCEPATH}

SOURCES += \
    check.cpp \
    abstractcheckdialog.cpp \
    checkkivdialog.cpp \
    checkkdv.cpp \
    check.cpp \
    checkktf.cpp \
    checkkdvdialog.cpp \
    checkktfdialog.cpp \
    checkkdvharmonicdialog.cpp \
    checkkdvvibrdialog.cpp \
    checkktfharmonicdialog.cpp \
    checkvibrkdv.cpp \
    checkkiv.cpp \
    checkharmonickdv.cpp \
    checkharmonicktf.cpp \

HEADERS += \
    check.h \
    checkharmonicktf.h \
    abstractcheckdialog.h \
    checkkivdialog.h \
    checkkdv.h \
    checkktf.h \
    checkkdvdialog.h \
    checkktfdialog.h \
    checkktfharmonicdialog.h \
    checkkdvharmonicdialog.h  \
    checkkdvvibrdialog.h \
    checkkiv.h \
    checkharmonickdv.h \
    checkvibrkdv.h \











