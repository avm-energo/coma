include( ../common.pri )
include( ../lib.pri )

QT += core gui widgets concurrent

TARGET = Widgets$${LIB_SUFFIX}
TEMPLATE = lib

DEFINES += WIDGETS_LIBRARY

SOURCES += \
    aboutwidget.cpp  \
    alarmbutton.cpp  \
    alarmwidget.cpp  \
    ecombobox.cpp  \
    edoublespinbox.cpp  \
    errorprotocolwidget.cpp \
    estackedwidget.cpp \
    etableview.cpp \
    etabwidget.cpp \
    lineeditfield.cpp \
    passwordlineedit.cpp \
    signalchoosewidget.cpp \
    splashscreen.cpp \
    styleloader.cpp \
    udialog.cpp \
    uwidget.cpp \
    waitwidget.cpp \
    wd_func.cpp



HEADERS += \
    aboutwidget.h \
    alarmbutton.h \
    alarmwidget.h \
    ecombobox.h \
    edoublespinbox.h \
    errorprotocolwidget.h \
    estackedwidget.h \
    etableview.h \
    etabwidget.h \
    keypress.h \
    lineeditfield.h \
    passwordlineedit.h \
    signalchoosewidget.h \
    splashscreen.h \
    styleloader.h \
    udialog.h \
    uwidget.h \
    waitwidget.h \
    wd_func.h

RESOURCES += \
    ../../images/vectorIcons.qrc \
    styles/QDarkStyleSheet/qdarkstyle/style.qrc \
    styles/styles.qrc


