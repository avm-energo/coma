file = $$PWD/gversion.h

!system(echo constexpr char gitHash[] = \"$$system(git rev-parse --short=8 HEAD)\" ; > $$file) {
  warning(Cannot create a file)
}
!system(echo constexpr unsigned long long gitCommitCounter = $$system(git rev-list --count HEAD); >> $$file) {
  warning(Cannot open a file)
}

include(../common.pri)
include(../lib.pri)
TEMPLATE=lib

TARGET = comaversion$${LIB_SUFFIX}

DEFINES += COMAVERSION_LIBRARY

include(comaversion.pri)
