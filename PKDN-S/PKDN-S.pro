#-------------------------------------------------
#
# Project created by QtCreator 2017-07-07T16:10:41
#
#-------------------------------------------------

QT       += core gui xlsx serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PKDN-S
TEMPLATE = app


SOURCES += main.cpp\
        pkdn_s.cpp \
    ../canal.cpp \
    ../commands.cpp \
    ../log.cpp \
    ../publicclass.cpp \
    ../widgets/errorprotocolwidget.cpp \
    ../widgets/getoscpbdelegate.cpp \
    ../widgets/messagebox.cpp \
    ../widgets/mystackedwidget.cpp \
    ../widgets/mytabwidget.cpp \
    ../widgets/s_tableitem.cpp \
    ../widgets/s_tablemodel.cpp \
    ../widgets/s_tqcheckbox.cpp \
    ../widgets/s_tqcombobox.cpp \
    ../widgets/s_tqspinbox.cpp \
    ../widgets/s_tqtableview.cpp \
    ../widgets/waitwidget.cpp \
    ../widgets/wd_func.cpp \
    ../dialogs/errordialog.cpp \
    ../dialogs/fwupdialog.cpp \
    ../dialogs/hiddendialog.cpp \
    ../dialogs/oscdialog.cpp \
    ../check/abstractcheckdialog.cpp \
    ../config/abstractconfdialog.cpp \
    ../config/confdialog.cpp \
    ../config/confdialoga1.cpp \
    ../config/config.cpp \
    ../config/configa1.cpp \
    ../check/checkdialoga1.cpp \
    ../tune/tunedialoga1.cpp \
    ../dialogs/keypressdialog.cpp \
    ../check/checka1.cpp \
    ../dialogs/settingsdialog.cpp \
    ../dialogs/downloaddialog.cpp

HEADERS  += pkdn_s.h \
    ../Canal.h \
    ../commands.h \
    ../log.h \
    ../publicclass.h \
    ../widgets/errorprotocolwidget.h \
    ../widgets/getoscpbdelegate.h \
    ../widgets/messagebox.h \
    ../widgets/mystackedwidget.h \
    ../widgets/mytabwidget.h \
    ../widgets/s_tableitem.h \
    ../widgets/s_tablemodel.h \
    ../widgets/s_tqcheckbox.h \
    ../widgets/s_tqcombobox.h \
    ../widgets/s_tqspinbox.h \
    ../widgets/s_tqtableview.h \
    ../widgets/waitwidget.h \
    ../widgets/wd_func.h \
    ../dialogs/errordialog.h \
    ../dialogs/fwupdialog.h \
    ../dialogs/hiddendialog.h \
    ../dialogs/oscdialog.h \
    ../check/abstractcheckdialog.h \
    ../config/abstractconfdialog.h \
    ../config/confdialog.h \
    ../config/confdialoga1.h \
    ../config/config.h \
    ../config/configa1.h \
    ../check/checkdialoga1.h \
    ../tune/tunedialoga1.h \
    ../dialogs/keypressdialog.h \
    ../check/checka1.h \
    ../dialogs/settingsdialog.h \
    ../dialogs/downloaddialog.h

RESOURCES += \
    pkdn-s.qrc