#include "tunekivmain.h"

#include "../../widgets/wd_func.h"

#include <QVBoxLayout>

TuneKIVMain::TuneKIVMain(int tuneStep, TuneKIV *kiv, QWidget *parent) : AbstractTuneDialog(tuneStep, parent)
{
    TKIV = kiv;
    //    m_tuneStep = 1;
    SetBac(&TKIV->m_Bac, 1, sizeof(TKIV->m_Bac));
    //    SetupUI();
}

void TuneKIVMain::setMessages()
{
    m_messages.append("1. Ввод пароля...");
    m_messages.append("2. Отображение предупреждения...");
    m_messages.append("3. Запрос настроечных параметров...");
}

void TuneKIVMain::setTuneFunctions()
{
    int count = 0;
    m_tuneFunctions[m_messages.at(count++)]
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::CheckPassword);
    Error::Msg (AbstractTuneDialog::*func)()
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVMain::showPreWarning);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::readTuneCoefs);
    m_tuneFunctions[m_messages.at(count++)] = func;
}

void TuneKIVMain::FillBac(int bacnum)
{
    Q_UNUSED(bacnum)
}

void TuneKIVMain::FillBackBac(int bacnum)
{
    Q_UNUSED(bacnum)
}

QWidget *TuneKIVMain::MainUI()
{
}

Error::Msg TuneKIVMain::showPreWarning()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;

    lyout->addWidget(WDFunc::NewLBL(this,
        "Данный этап регулировки должен выполняться при температуре"
        "окружающего воздуха +20±7 °С"));
    lyout->addWidget(WDFunc::NewLBL(this,
        "Если температура окружающего воздуха отличается от указанной,"
        "разместите модуль в термокамеру с диапазоном регулирования температуры "
        "от минус 20 до +60°С. Установите нормальное значение температуры "
        "в камере 20±5°С"));
    lyout->addWidget(WDFunc::NewPB2(this, "", "Готово", [dlg] { dlg->close(); }));
    lyout->addWidget(WDFunc::NewPB2(this, "cancelpb", "Отмена", [dlg] { dlg->close(); }));
    dlg->setLayout(lyout);
    WDFunc::PBConnect(dlg, "cancelpb", this, &AbstractTuneDialog::CancelTune);
    dlg->exec();
    return Error::Msg::NoError;
}

// void TuneKIVMain::GetBdAndFill()
//{
//}

// int TuneKIVMain::ReadAnalogMeasurements()
//{
//    return 0;
//}

// void TuneKIVMain::SetDefCoefs()
//{
//}
