include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets serialport concurrent

CONFIG -= c++2a
CONFIG += c++17

TARGET = dialogs$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += DIALOGS_LIBRARY

include(dialogs.pri)