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
            AVM-Service \
            AVM-Debug

AVM-Service.depends = check config datablocks dialogs interfaces models module startup widgets gen
AVM-Debug.depends = check config datablocks dialogs interfaces models module startup widgets gen tune
