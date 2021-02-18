include(common.pri)

#include(gen\gen.pri)
QT       += core gui printsupport network serialport widgets concurrent testlib xml
QT.testlib.CONFIG -= console
TARGET = AVM-Service
VERSION = 2.1

TEMPLATE = app

RC_ICONS = ../coma.ico

DEFINES -= QT_DISABLE_DEPRECATED_BEFORE=0x060000

QMAKE_SUBSTITUTES += config.h.in

INCLUDEPATH += $${QXLSX_PARENTPATH}
INCLUDEPATH += $${QXLSX_HEADERPATH}
INCLUDEPATH += $${QXLSX_SOURCEPATH}

include(../include/QXlsx/QXlsx/QXlsx.pri)


unix {
LIBS += -lhidapi-hidraw
contains(QT_ARCH, x86_64) {
        message("Unix x64 build")
        ## Unix x64 (64bit) specific build here
        CONFIG(debug, debug|release) {

        }
    } else {
        message("Unix x86 build")
        ## Unix x86 (32bit) specific build here
        CONFIG(debug, debug|release) {

        }
    }
    -fsanitize=address -fno-omit-frame-pointer
}

HEADERS += \
    AVM-Service/servicecoma.h \
    AVM-Service/servicemodule.h \
    AVM-Service/settings.h \
    avtuk/eoscillogram.h \
    avtuk/getoscpbdelegate.h \
    avtuk/oscdialog.h \
    avtuk/oscmanager.h \
    avtuk/parseid10031.h \
    avtuk/parseid9000.h \
    avtuk/parseid9050.h \
    avtuk/parsemodule.h \
    avtuk/switchjournaldialog.h \
    avtuk/swjdialog.h \
    avtuk/trendviewdialog.h \
    avtuk/trendviewmodel.h \
    check/abstractcheckdialog.h \
    check/check.h \
    check/checkharmonickdv.h \
    check/checkharmonicktf.h \
    check/checkkdv.h \
    check/checkkdvdialog.h \
    check/checkkdvharmonicdialog.h \
    check/checkkdvvibrdialog.h \
    check/checkkiv.h \
    check/checkkivdialog.h \
    check/checkktf.h \
    check/checkktfdialog.h \
    check/checkktfharmonicdialog.h \
    config/abstractconfdialog.h \
    config/config.h \
    config/configkdv.h \
    config/configkiv.h \
    config/configktf.h \
    config/configkxx.h \
    config/confkdvdialog.h \
    config/confkivdialog.h \
    config/confktfdialog.h \
    dialogs/abstractinterfacedialog.h \
    dialogs/connectdialog.h \
    dialogs/errordialog.h \
    dialogs/fwuploaddialog.h \
    dialogs/infodialog.h \
    dialogs/interfaceethernetdialog.h \
    dialogs/interfaceserialdialog.h \
    dialogs/interfaceusbdialog.h \
    dialogs/journalsdialog.h \
    dialogs/keypressdialog.h \
    dialogs/settingsdialog.h \
    dialogs/timedialog.h \
    gen/board.h \
    gen/colors.h \
    gen/datamanager.h \
    gen/datatypes.h \
    gen/error.h \
    gen/errorqueue.h \
    gen/files.h \
    gen/helper.h \
    gen/logclass.h \
    gen/logger.h \
    gen/pch.h \
    gen/s2.h \
    gen/singleton.h \
    gen/stdfunc.h \
    gen/timefunc.h \
    interfaces/baseinterface.h \
    interfaces/iec104.h \
    interfaces/iec104private.h \
    interfaces/iec104thread.h \
    interfaces/interfacesettings.h \
    interfaces/modbus.h \
    interfaces/modbusprivate.h \
    interfaces/modbusthread.h \
    interfaces/protocom.h \
    interfaces/protocom_p.h \
    interfaces/protocomprivate.h \
    interfaces/protocomthread.h \
    interfaces/serialport.h \
    interfaces/settingstypes.h \
    interfaces/usbhidport.h \
    interfaces/usbhidportinfo.h \
    interfaces/usbhidsettings.h \
    models/abstracttablerow.h \
    models/errorprotocolitem.h \
    models/errorprotocolmodel.h \
    models/etablemodel.h \
    models/etablerow.h \
    module/alarmstateall.h \
    module/basealarm.h \
    module/journals.h \
    module/journkdv.h \
    module/journkiv.h \
    module/journktf.h \
    module/module.h \
    module/module_kiv.h \
    module/module_kxx.h \
    module/modulealarm.h \
    module/modules.h \
    module/registers.h \
    qcustomplot/qcustomplot.h \
    startup/abstractstartupdialog.h \
    startup/startupkdvdialog.h \
    startup/startupkivdialog.h \
    startup/startupktfdialog.h \
    widgets/aboutwidget.h \
    widgets/alarmbutton.h \
    widgets/alarmwidget.h \
    widgets/coma.h \
    widgets/ecombobox.h \
    widgets/edoublespinbox.h \
    widgets/errorprotocolwidget.h \
    widgets/estackedwidget.h \
    widgets/etableview.h \
    widgets/etabwidget.h \
    widgets/keypress.h \
    widgets/lineeditfield.h \
    widgets/passwordlineedit.h \
    widgets/signalchoosewidget.h \
    widgets/splashscreen.h \
    widgets/styleloader.h \
    widgets/udialog.h \
    widgets/uwidget.h \
    widgets/waitwidget.h \
    widgets/wd_func.h

SOURCES += \
    AVM-Service/main.cpp \
    AVM-Service/servicecoma.cpp \
    AVM-Service/servicemodule.cpp \
    AVM-Service/settings.cpp \
    avtuk/eoscillogram.cpp \
    avtuk/getoscpbdelegate.cpp \
    avtuk/oscdialog.cpp \
    avtuk/oscmanager.cpp \
    avtuk/parseid10031.cpp \
    avtuk/parseid9000.cpp \
    avtuk/parseid9050.cpp \
    avtuk/parsemodule.cpp \
    avtuk/switchjournaldialog.cpp \
    avtuk/swjdialog.cpp \
    avtuk/trendviewdialog.cpp \
    avtuk/trendviewmodel.cpp \
    check/abstractcheckdialog.cpp \
    check/check.cpp \
    check/checkharmonickdv.cpp \
    check/checkharmonicktf.cpp \
    check/checkkdv.cpp \
    check/checkkdvdialog.cpp \
    check/checkkdvharmonicdialog.cpp \
    check/checkkdvvibrdialog.cpp \
    check/checkkiv.cpp \
    check/checkkivdialog.cpp \
    check/checkktf.cpp \
    check/checkktfdialog.cpp \
    check/checkktfharmonicdialog.cpp \
    config/abstractconfdialog.cpp \
    config/config.cpp \
    config/configkdv.cpp \
    config/configkiv.cpp \
    config/configktf.cpp \
    config/configkxx.cpp \
    config/confkdvdialog.cpp \
    config/confkivdialog.cpp \
    config/confktfdialog.cpp \
    dialogs/abstractinterfacedialog.cpp \
    dialogs/connectdialog.cpp \
    dialogs/errordialog.cpp \
    dialogs/fwuploaddialog.cpp \
    dialogs/infodialog.cpp \
    dialogs/interfaceethernetdialog.cpp \
    dialogs/interfaceserialdialog.cpp \
    dialogs/interfaceusbdialog.cpp \
    dialogs/journalsdialog.cpp \
    dialogs/keypressdialog.cpp \
    dialogs/settingsdialog.cpp \
    dialogs/timedialog.cpp \
    gen/board.cpp \
    gen/datamanager.cpp \
    gen/error.cpp \
    gen/errorqueue.cpp \
    gen/files.cpp \
    gen/helper.cpp \
    gen/logclass.cpp \
    gen/logger.cpp \
    gen/pch.cpp \
    gen/pch.h.cpp \
    gen/s2.cpp \
    gen/stdfunc.cpp \
    gen/timefunc.cpp \
    interfaces/baseinterface.cpp \
    interfaces/iec104.cpp \
    interfaces/iec104thread.cpp \
    interfaces/interfacesettings.cpp \
    interfaces/modbus.cpp \
    interfaces/modbusprivate.cpp \
    interfaces/modbusthread.cpp \
    interfaces/protocom.cpp \
    interfaces/protocom_p.cpp \
    interfaces/protocomthread.cpp \
    interfaces/serialport.cpp \
    interfaces/usbhidport.cpp \
    interfaces/usbhidportinfo.cpp \
    interfaces/usbhidsettings.cpp \
    models/errorprotocolitem.cpp \
    models/errorprotocolmodel.cpp \
    models/etablemodel.cpp \
    models/etablerow.cpp \
    module/alarmstateall.cpp \
    module/basealarm.cpp \
    module/journals.cpp \
    module/journkdv.cpp \
    module/journkiv.cpp \
    module/journktf.cpp \
    module/module.cpp \
    module/modulealarm.cpp \
    qcustomplot/qcustomplot.cpp \
    startup/abstractstartupdialog.cpp \
    startup/startupkdvdialog.cpp \
    startup/startupkivdialog.cpp \
    startup/startupktfdialog.cpp \
    widgets/aboutwidget.cpp \
    widgets/alarmbutton.cpp \
    widgets/alarmwidget.cpp \
    widgets/coma.cpp \
    widgets/ecombobox.cpp \
    widgets/edoublespinbox.cpp \
    widgets/errorprotocolwidget.cpp \
    widgets/estackedwidget.cpp \
    widgets/etableview.cpp \
    widgets/etabwidget.cpp \
    widgets/lineeditfield.cpp \
    widgets/passwordlineedit.cpp \
    widgets/signalchoosewidget.cpp \
    widgets/splashscreen.cpp \
    widgets/styleloader.cpp \
    widgets/udialog.cpp \
    widgets/uwidget.cpp \
    widgets/waitwidget.cpp \
    widgets/wd_func.cpp

RESOURCES += \
    ../images/vectorIcons.qrc \
    widgets/styles/QDarkStyleSheet/qdarkstyle/style.qrc \
    widgets/styles/styles.qrc
