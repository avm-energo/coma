
CONFIG += c++17
CONFIG -= console

QT += core concurrent gui widgets

TEMPLATE = SUBDIRS

SOURCES += \
    abstracttunedialog.cpp \
    generaltunedialog.cpp \



HEADERS += \
    abstracttunedialog.h \
    generaltunedialog.h \
    tunesteps.h


