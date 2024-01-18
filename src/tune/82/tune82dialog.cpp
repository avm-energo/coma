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

Tune82Dialog::Tune82Dialog(S2::Configuration &config, Modules::MezzanineBoard typeM, QWidget *parent)
    : GeneralTuneDialog(config, parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QString step2Caption = "Регулировка каналов %1";
    switch (typeM)
    {
    case Modules::MezzanineBoard::MTM_81:
        step2Caption = step2Caption.arg("тока");
        break;
    case Modules::MezzanineBoard::MTM_82:
        step2Caption = step2Caption.arg("тока и напряжения");
        break;
    case Modules::MezzanineBoard::MTM_83:
        step2Caption = step2Caption.arg("напряжения");
        break;
    default:
        assert(false && "Undefined mezzanine board");
        break;
    }
    m_dialogList = {
        { "Проверка правильности измерения входных сигналов", new Tune82Check(config, TS82_CHECKING, typeM, this) }, //
        { step2Caption, new Tune82ADC(config, typeM, TS82_ADC, this) },                                              //
        { "Поверка", new Tune82Verification(config, typeM, TS82_VERIFICATION, this) }                                //
    };

    m_calibrSteps = m_dialogList.size() + 1;
    Bac82 *bac = new Bac82;
    addWidgetToTabWidget(bac->widget(), "Регулировка");
    SetupUI();
}

void Tune82Dialog::prepareReport()
{
    m_reporter->setTemplatePath(StdFunc::GetSystemHomeDir() + "82report.lrxml");
}
