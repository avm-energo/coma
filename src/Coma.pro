TEMPLATE = subdirs

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
            ctti \
            qcustomplot \
            avtuk \
            tests \
            AVM-Service \
            AVM-Debug

AVM-Service.depends = check config datablocks dialogs interfaces models module startup widgets gen avtuk qcustomplot
AVM-Debug.depends = check config datablocks dialogs interfaces models module startup widgets gen tune avtuk qcustomplot
tests.depends = check config datablocks dialogs interfaces models module startup widgets gen tune avtuk
