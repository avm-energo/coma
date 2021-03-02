TEMPLATE = subdirs

#include(gen\gen.pri)

CONFIG += ordered

SUBDIRS = \
            check \   # relative paths
            config \
            datablocks \
            dialogs \
            interfaces \
            models \
            module \
            startup \
            widgets \
            gen \
            app

app.target = apptarget

QMAKE_EXTRA_TARGETS += apptarget
