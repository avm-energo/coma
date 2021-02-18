include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets

TARGET = models$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += MODELS_LIBRARY

include(models.pri)
