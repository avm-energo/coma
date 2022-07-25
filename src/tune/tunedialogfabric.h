#ifndef TUNEDIALOGFABRIC_H
#define TUNEDIALOGFABRIC_H

#include "../s2/configv.h"
#include "generaltunedialog.h"

#include <QObject>

class TuneDialogFabric
{
public:
    TuneDialogFabric();

    static void setConfigV(ConfigV *config);
    static GeneralTuneDialog *GetTuneDialogForType(quint16 typeB, quint16 typeM);

private:
    static ConfigV *s_Config;
};

#endif // TUNEDIALOGFABRIC_H
