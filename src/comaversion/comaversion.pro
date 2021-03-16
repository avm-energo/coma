file = $$PWD/version.txt
!system(echo $$system(git rev-parse --short=8 HEAD) > $$file) {
  warning(Cannot create a file)
}
include(../common.pri)
include(../lib.pri)
TEMPLATE=lib

TARGET = COMAVERSION$${LIB_SUFFIX}


DEFINES += COMAVERSION_LIBRARY


RESOURCES += \
    helper.qrc
