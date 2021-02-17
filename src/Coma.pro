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
            qcustomplot \
            avtuk \
            AVM-Service \
            AVM-Debug

AVM-Service.depends = check config datablocks dialogs interfaces models module startup widgets gen avtuk qcustomplot
AVM-Debug.depends = check config datablocks dialogs interfaces models module startup widgets gen tune avtuk qcustomplot
