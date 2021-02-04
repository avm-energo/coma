include( ../common.pri )

QT       += core gui printsupport network serialport widgets concurrent testlib xml
QT.testlib.CONFIG -= console
TARGET = AVM-Service
VERSION = 2.0.0

TEMPLATE = app

QMAKE_SUBSTITUTES += ../config.h.in

INCLUDEPATH += ../$${QXLSX_PARENTPATH}
INCLUDEPATH += ../$${QXLSX_HEADERPATH}
INCLUDEPATH += ../$${QXLSX_SOURCEPATH}

SOURCES += \
    coma.cpp \
    main.cpp \
    servicemodule.cpp
	
HEADERS += \
   coma.h \
    servicemodule.h

DESTDIR = $${BIN_PATH}/$${TARGET}
win32 {
    LIBS += -luser32
    LIBS += -L$$PWD/../../lib/$${QT_ARCH}/$${BUILD_FLAG}/ -lhidapi
    CONFIG(release, debug|release) {
        LIBS_FILES += \
        $$PWD/../../lib/$${QT_ARCH}/$${BUILD_FLAG}/hidapi.dll \
    }
    message("Windows "$${QT_ARCH}" build "$${TARGET})
}

LIBRARIES += check \
    config \
    datablock \
    dialogs \
    interfaces \
    models \
    module \
    startup \
    widgets \
    general

include(../deps.pri)

IMAGE_FILE += \
    $$PWD/../../images

ERROR_FILES += \
    $$PWD/../../errors

copyToDestDir($$IMAGE_FILE, $$DESTDIR/images/)
copyToDestDir($$ERROR_FILES, $$DESTDIR/errors/)
copyToDestDir($$LIBS_FILES, $$DESTDIR/)
