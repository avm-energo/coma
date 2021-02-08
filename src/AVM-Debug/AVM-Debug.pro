include( ../common.pri )

QT       += core gui printsupport network serialport widgets concurrent testlib xml qml svg
QT.testlib.CONFIG -= console
TARGET = AVM-Debug
VERSION = 2.0.0

TEMPLATE = app

QMAKE_SUBSTITUTES += config.h.in

INCLUDEPATH += ../$${QXLSX_PARENTPATH}
INCLUDEPATH += ../$${QXLSX_HEADERPATH}
INCLUDEPATH += ../$${QXLSX_SOURCEPATH}
INCLUDEPATH += ../$${LIME_HEADERPATH}

SOURCES += \
    coma.cpp \
    main.cpp \
    tunemodule.cpp
	
HEADERS += \
   coma.h \
    tunemodule.h

DESTDIR = $${BIN_PATH}/$${TARGET}
win32 {
    LIBS += -luser32

    LIBS += -L$$PWD/../../lib/$${QT_ARCH}/$${BUILD_FLAG}/ -llimereport$${LIB_SUFFIX} -lhidapi
    CONFIG(release, debug|release) {
        LIBS_FILES += \
        $$PWD/../../lib/$${QT_ARCH}/$${BUILD_FLAG}/hidapi.dll \
        $$PWD/../../lib/$${QT_ARCH}/$${BUILD_FLAG}/limereport.dll \
        $$PWD/../../lib/$${QT_ARCH}/$${BUILD_FLAG}/QtZint.dll
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
    general \
    tune

include(../deps.pri)

IMAGE_FILE += \
    $$PWD/../../images

ERROR_FILES += \
    $$PWD/../../errors



copyToDestDir($$IMAGE_FILE, $$DESTDIR/images/)
copyToDestDir($$ERROR_FILES, $$DESTDIR/errors/)
copyToDestDir($$LIBS_FILES, $$DESTDIR/)

