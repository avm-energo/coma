#include <comaresources/manage.h>

void initResources()
{
    Q_INIT_RESOURCE(darkstyle);
    Q_INIT_RESOURCE(lightstyle);
    Q_INIT_RESOURCE(styles);
    Q_INIT_RESOURCE(images);
}

void freeResources()
{
    Q_CLEANUP_RESOURCE(darkstyle);
    Q_CLEANUP_RESOURCE(lightstyle);
    Q_CLEANUP_RESOURCE(styles);
    Q_CLEANUP_RESOURCE(images);
}
