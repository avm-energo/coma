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
            tune \
            AVM-Service \
            AVM-Debug

AVM-Service.target = servicetarget
AVM-Debug.target = debugtarget

QMAKE_EXTRA_TARGETS += servicetarget debugtarget
