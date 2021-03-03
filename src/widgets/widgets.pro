include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets

TARGET = widgets$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += WIDGETS_LIBRARY

include(widgets.pri)

RESOURCES += \
    ../../images/vectorIcons.qrc \
    styles/QDarkStyleSheet/qdarkstyle/style.qrc \
    styles/styles.qrc


