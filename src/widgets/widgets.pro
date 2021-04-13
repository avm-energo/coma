include( ../common.pri )
include( ../lib.pri )
CONFIG += conan_basic_setup
include($${PWD}/../../conanbuildinfo.pri)

QT += core gui widgets

TARGET = widgets$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += WIDGETS_LIBRARY

include(widgets.pri)

RESOURCES += \
    ../../images/vectorIcons.qrc \
    styles/QDarkStyleSheet/qdarkstyle/style.qrc \
    styles/styles.qrc


