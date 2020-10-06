#include "tunekivcheck.h"

#include "../../usb/commands.h"

TuneKIVCheck::TuneKIVCheck(TuneKIV *kiv, QWidget *parent) : AbstractTuneDialog(parent)
{
    TKIV = kiv;
    S2Config = TKIV->getS2Config();
    m_tuneStep = 1;
    SetupUI();
}

void TuneKIVCheck::SetupUI()
{
}

void TuneKIVCheck::SetLbls()
{
    lbls.append("1. Ввод пароля...");
    lbls.append("2. Сохранение текущей конфигурации...");
    lbls.append("7.2.1. Задание максимальных токов 600 мА...");
    lbls.append("7.2.2. Отображение схемы подключения...");
    lbls.append("7.2.7. Получение результатов измерений...");
}

void TuneKIVCheck::SetPf()
{
    int count = 0;
    pf[lbls.at(count++)] = &AbstractTuneDialog::CheckPassword;
    Error::Msg (AbstractTuneDialog::*func)()
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVCheck::SaveWorkConfig);
    pf[lbls.at(count++)] = func;
    /*    func = reinterpret_cast<Error::Msg (EAbstractTuneDialog::*)()>(&TuneKIVCheck::Start7_2_1);
        pf[lbls.at(count++)] = func;
        func = reinterpret_cast<Error::Msg (EAbstractTuneDialog::*)()>(&TuneKIVCheck::ShowScheme);
        pf[lbls.at(count++)] = func;
        func = reinterpret_cast<Error::Msg (EAbstractTuneDialog::*)()>(&TuneKIVCheck::Start_7_2_7);
        pf[lbls.at(count++)] = func; */
}

void TuneKIVCheck::FillBac(int bacnum)
{
    Q_UNUSED(bacnum)
}

void TuneKIVCheck::FillBackBac(int bacnum)
{
    Q_UNUSED(bacnum)
}

void TuneKIVCheck::GetBdAndFill()
{
}

int TuneKIVCheck::ReadAnalogMeasurements()
{
    return 0;
}

void TuneKIVCheck::SetDefCoefs()
{
}
