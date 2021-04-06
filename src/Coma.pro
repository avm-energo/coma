TEMPLATE = subdirs

CONFIG =+ ordered

SUBDIRS = \
            comaversion \
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
            ctti \
            qcustomplot \
            avtuk \
            tests \
            AVM-Service \
            AVM-Debug


AVM-Service.depends = comaversion check config datablocks dialogs interfaces models module startup widgets gen avtuk qcustomplot
AVM-Debug.depends = comaversion check config datablocks dialogs interfaces models module startup widgets gen tune avtuk qcustomplot
tests.depends = comaversion check config datablocks dialogs interfaces models module startup widgets gen tune avtuk
