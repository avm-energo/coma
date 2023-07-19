#ifndef TUNEDIALOGFABRIC_H
#define TUNEDIALOGFABRIC_H

#include "generaltunedialog.h"

#include <QObject>

class TuneDialogFabric
{
public:
    TuneDialogFabric();

    // static void setConfigV(ConfigV *config);
    static UDialog *GetTuneDialogForType(Modules::BaseBoard typeB, Modules::MezzanineBoard typeM);

private:
    // static ConfigV *s_Config;
};

#endif // TUNEDIALOGFABRIC_H
