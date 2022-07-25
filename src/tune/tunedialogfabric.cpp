#include "tunedialogfabric.h"

#include "../module/board.h"
#include "../module/modules.h"
#include "82/tune82dialog.h"
#include "84/tune84dialog.h"
#include "kiv/tunekivdialog.h"

TuneDialogFabric::TuneDialogFabric()
{
}

void TuneDialogFabric::setConfigV(ConfigV *config)
{
    s_Config = config;
}

GeneralTuneDialog *TuneDialogFabric::GetTuneDialogForType(quint16 typeB, quint16 typeM)
{
    switch (typeB)
    {
    case Modules::BaseBoard::MTB_80:
    {
        switch (typeM)
        {
        case Modules::MezzanineBoard::MTM_82:
            return new Tune82Dialog(s_Config);
        case Modules::MezzanineBoard::MTM_84:
            return new Tune84Dialog(s_Config);
        default:
            return 0;
        }
    case Modules::BaseBoard::MTB_A2:
    {
        switch (typeM)
        {
        case Modules::MezzanineBoard::MTM_84:
            return new TuneKIVDialog(s_Config);
        default:
            return 0;
        }
    }
    default:
        return 0;
    }
    }
    return 0;
}
