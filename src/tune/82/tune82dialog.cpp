#include "tune82dialog.h"

#include "../../widgets/wd_func.h"
#include "../tunesteps.h"
#include "tune82adc.h"
#include "tune82check.h"
#include "tune82verification.h"
#include "tunereporter.h"

#include <gen/colors.h>
#include <gen/error.h>
#include <gen/files.h>
#include <gen/stdfunc.h>

Tune82Dialog::Tune82Dialog(Device::CurrentDevice *device, QWidget *parent) : GeneralTuneDialog(device, parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    QString step2Caption = "Регулировка каналов %1";
    auto typeM = static_cast<Device::MezzanineBoard>(device->getMezzType());
    switch (typeM)
    {
    case Device::MezzanineBoard::MTM_81:
        step2Caption = step2Caption.arg("тока");
        break;
    case Device::MezzanineBoard::MTM_82:
        step2Caption = step2Caption.arg("тока и напряжения");
        break;
    case Device::MezzanineBoard::MTM_83:
        step2Caption = step2Caption.arg("напряжения");
        break;
    default:
        assert(false && "Undefined mezzanine board");
        break;
    }
    m_dialogList = {
        { "Проверка правильности измерения входных сигналов", new Tune82Check(TS82_CHECKING, device, this) }, //
        { step2Caption, new Tune82ADC(TS82_ADC, device, this) },                                              //
        { "Поверка", new Tune82Verification(TS82_VERIFICATION, device, this) }                                //
    };

    m_calibrSteps = m_dialogList.size() + 1;
    Bac82 *bac = new Bac82(this);
    bac->setup(m_device->getUID(), m_device->sync());
    addWidgetToTabWidget(bac->widget(), "Регулировка");
    SetupUI();
}

void Tune82Dialog::prepareReport()
{
    m_reporter->setTemplatePath(StdFunc::GetSystemHomeDir() + "reports/82report.lrxml");
}
