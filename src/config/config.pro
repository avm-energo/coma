include( ../common.pri )
include( ../lib.pri )
CONFIG += conan_basic_setup
include($${PWD}/../../conanbuildinfo.pri)
QT += core gui widgets

TARGET = config$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += CONFIG_LIBRARY

include(config.pri)

