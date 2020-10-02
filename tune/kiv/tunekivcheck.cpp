#include "tunekivcheck.h"
#include "../../usb/commands.h"

TuneKIVCheck::TuneKIVCheck(TuneKIV *kiv, QWidget *parent) : EAbstractTuneDialog(parent)
{
    TKIV = kiv;
    m_tuneStep = 1;
    SetupUI();
}

void TuneKIVCheck::SetupUI() { }

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
    pf[lbls.at(count++)] = &EAbstractTuneDialog::CheckPassword;
    Error::Msg (EAbstractTuneDialog::*func)()
        = reinterpret_cast<Error::Msg (EAbstractTuneDialog::*)()>(&TuneKIVCheck::SaveWorkConfig);
    pf[lbls.at(count++)] = func;
    /*    func = reinterpret_cast<Error::Msg (EAbstractTuneDialog::*)()>(&TuneKIVCheck::Start7_2_1);
        pf[lbls.at(count++)] = func;
        func = reinterpret_cast<Error::Msg (EAbstractTuneDialog::*)()>(&TuneKIVCheck::ShowScheme);
        pf[lbls.at(count++)] = func;
        func = reinterpret_cast<Error::Msg (EAbstractTuneDialog::*)()>(&TuneKIVCheck::Start_7_2_7);
        pf[lbls.at(count++)] = func; */
}

void TuneKIVCheck::FillBac(int bacnum) { Q_UNUSED(bacnum) }

void TuneKIVCheck::FillBackBac(int bacnum) { Q_UNUSED(bacnum) }

void TuneKIVCheck::GetBdAndFill() { }

Error::Msg TuneKIVCheck::SaveWorkConfig()
{
    /*    if (Commands::GetFileWithRestore(CM_CONFIGFILE, TKIV S2ConfigForTune) == NOERROR)
            memcpy(&m_Bci_block_work, &CKIV->Bci_block, sizeof(ConfigKIV::Bci));
        else
            return GENERALERROR; */
    return Error::Msg::NoError;
}
