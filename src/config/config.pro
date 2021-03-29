include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets

TARGET = config$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += CONFIG_LIBRARY

include(config.pri)

