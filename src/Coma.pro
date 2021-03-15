TEMPLATE = subdirs

SUBDIRS = \
            check \   # relative paths
            config \
            comaversion \
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


AVM-Service.depends = check config datablocks dialogs interfaces models module startup widgets gen avtuk qcustomplot comaversion
AVM-Debug.depends = check config datablocks dialogs interfaces models module startup widgets gen tune avtuk qcustomplot comaversion
tests.depends = check config datablocks dialogs interfaces models module startup widgets gen tune avtuk
