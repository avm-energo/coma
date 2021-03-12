#include "tune84check.h"

#include "../../datablocks/kiv/bda.h"
#include "../../gen/files.h"
#include "../../gen/stdfunc.h"
#include "../../interfaces/protocom.h"
#include "../../widgets/wd_func.h"

#include <QDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

Tune84Check::Tune84Check(int tuneStep, QWidget *parent) : AbstractTuneDialog(tuneStep, parent)
{
    SetupUI();
}

void Tune84Check::setMessages()
{
    m_messages.append("1. Ввод пароля...");
    m_messages.append("2. Сохранение текущей конфигурации...");
    m_messages.append("3. Отображение схемы подключения...");
    m_messages.append("4. Проверка...");
}

void Tune84Check::setTuneFunctions()
{
    int count = 0;
    m_tuneFunctions[m_messages.at(count++)]
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::CheckPassword);
    Error::Msg (AbstractTuneDialog::*func)()
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::saveWorkConfig);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84Check::showScheme);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&Tune84Check::check);
    m_tuneFunctions[m_messages.at(count++)] = func;
}

Error::Msg Tune84Check::showScheme()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;

    lyout->addWidget(WDFunc::NewLBL(this, "", "", "", new QPixmap("images/tunekiv1.png")));
    lyout->addWidget(WDFunc::NewLBL(this, "1. Соберите схему подключения по одной из вышеприведённых картинок;"));
    lyout->addWidget(WDFunc::NewLBL(this,
        "2. Включите питание Энергомонитор 3.1КМ и настройте его на режим измерения тока"
        "и напряжения в однофазной сети переменного тока, установите предел измерения"
        "по напряжению 60 В, по току - 2,5 А;"));
    lyout->addWidget(WDFunc::NewLBL(this,
        "3. Включите питание источника сигналов. При работе с имитатором ИС АВМ-КИВ задайте напряжение U1 равным 60,0 "
        "В,"
        "ток I1 равным 29 мА, tgδ равным +0,2 %;\nПри работе с РЕТОМ-51 задайте напряжение U1 равным 60,0 В с углом 0 "
        "град.,"
        "ток I1 равным 5 А с углом 89,9 град., включите выходы РЕТОМ-51;"));
    lyout->addWidget(WDFunc::NewLBL(this,
        "4. По показаниям Энергомонитора убедитесь, что входное напряжение от источника"
        "составляет 60 ± 0,25 В, ток – 0,500 ± 25 мА, частота – 51,0 ± 0,05 Гц;"));
    lyout->addWidget(WDFunc::NewLBL(
        this, "6. Убедитесь, что частота мигания светодиода «Работа»  на лицевой панели увеличилась до 1 Гц;"));
    lyout->addWidget(WDFunc::NewPB(this, "", "Готово", [dlg] { dlg->close(); }));
    lyout->addWidget(WDFunc::NewPB(this, "cancelpb", "Отмена", [dlg] { dlg->close(); }));
    dlg->setLayout(lyout);
    WDFunc::PBConnect(dlg, "cancelpb", static_cast<AbstractTuneDialog *>(this), &AbstractTuneDialog::CancelTune);
    dlg->exec();
    return Error::Msg::NoError;
}

Error::Msg Tune84Check::check()
{
    Bda *bda = new Bda;
    bda->readAndUpdate();
#ifndef NO_LIMITS
    for (int i = 0; i < 3; ++i)
        if (!StdFunc::floatIsWithinLimits(this, bda->data()->Ueff_ADC[i], 2150000.0, 150000.0))
            return Error::Msg::GeneralError;
    for (int i = 3; i < 6; ++i)
        if (!StdFunc::floatIsWithinLimits(this, bda->data()->Ueff_ADC[i], 1220000.0, 60000.0))
            return Error::Msg::GeneralError;
    if (!StdFunc::floatIsWithinLimits(this, bda->data()->Frequency, 51.0, 0.05))
        return Error::Msg::GeneralError;
#endif
    return Error::Msg::NoError;
}

void Tune84Check::showEvent(QShowEvent *e)
{
    e->accept();
}
