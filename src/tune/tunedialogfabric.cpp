#include "tunedialogfabric.h"

#include "../module/board.h"
#include "../module/modules.h"
#include "82/tune82dialog.h"
#include "84/tune84dialog.h"
#include "kiv/tunekivdialog.h"

TuneDialogFabric::TuneDialogFabric()
{
}

AbstractTuneDialog *TuneDialogFabric::GetTuneDialogForType(quint16 typeB, quint16 typeM)
{
    switch (typeB)
    {
    case Modules::BaseBoard::MTB_80:
    {
        switch (typeM)
        {
        case Modules::MezzanineBoard::MTM_82:
            return new Tune82Dialog();
        }
    }
    }
}
