#include "tuneclass.h"

#include "../../widgets/wd_func.h"
#include "../gen/board.h"
#include "../gen/stdfunc.h"

#include <QGroupBox>
#include <QMessageBox>
#include <QVBoxLayout>

S2DataTypes::S2ConfigType *TuneClass::getS2Config()
{
    return S2Config;
}

void TuneClass::ReadTuneCoefs()
{
}

bool TuneClass::WriteTuneCoefsSlot()
{
    return true;
}
