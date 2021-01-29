include( ../common.pri )
include( ../lib.pri )
QT += core gui widgets

TARGET = Check$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += CHECK_LIBRARY

# QXlsx code for Application Qt project
QXLSX_PARENTPATH=./../../include/QXlsx/QXlsx         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./../../include/QXlsx/QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./../../include/QXlsx/QXlsx/source/  # current QXlsx source path is ./source/
include(./../../include/QXlsx/QXlsx/QXlsx.pri)

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











