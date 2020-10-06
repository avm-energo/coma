#include "tuneclass.h"

#include "../../usb/commands.h"
#include "../../widgets/wd_func.h"
#include "../gen/board.h"
#include "../gen/stdfunc.h"

#include <QGroupBox>
#include <QMessageBox>
#include <QVBoxLayout>

TuneClass::TuneClass(int bacnum, S2ConfigType *S2Config, QObject *parent) : QObject(parent)
{
    this->S2Config = S2Config;
    m_VModel = new ValueModel;
    m_BacNum = bacnum;
}

void TuneClass::update()
{
    m_VModel->updateModel();
}

void TuneClass::getFromWidget()
{
    m_VModel->updateFromModel();
}

S2ConfigType *TuneClass::getS2Config()
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
