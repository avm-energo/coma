#include "tunekivmain.h"

#include "../../gen/colors.h"
#include "../../gen/stdfunc.h"
#include "../../widgets/waitwidget.h"
#include "../../widgets/wd_func.h"

#include <QMessageBox>
#include <QVBoxLayout>

TuneKIVMain::TuneKIVMain(int tuneStep, ConfigKIV *ckiv, TuneKIV *kiv, QWidget *parent)
    : AbstractTuneDialog(tuneStep, parent)
{
    CKIV = ckiv;
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
    m_messages.append("4. Проверка настроечных параметров...");
    m_messages.append("5. Настройка канала измерения температуры (КИТ): установка 80 Ом...");
    m_messages.append("6. Настройка КИТ: обработка...");
    m_messages.append("7. Настройка канала измерения температуры (КИТ): установка 120 Ом...");
    m_messages.append("8. Настройка КИТ: обработка и запись коэффициентов...");
    m_messages.append("9. Сохранение конфигурации...");
    m_messages.append("10. Регулировка для Кацп = 1...");
    m_messages.append("11. Отображение диалога задания входных данных...");
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
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVMain::checkTuneCoefs);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVMain::setR80);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVMain::processR80);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVMain::setR120);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVMain::processR120);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::saveWorkConfig);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVMain::ADCCoef1);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVMain::showDialog1);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVMain::setADCCoef2);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVMain::setADCCoef4);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVMain::setADCCoef8);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVMain::setADCCoef16);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVMain::setADCCoef32);
    m_tuneFunctions[m_messages.at(count++)] = func;
}

void TuneKIVMain::FillBac(int bacnum)
{
    TKIV->updateBacWidget();
    Q_UNUSED(bacnum)
}

void TuneKIVMain::FillBackBac(int bacnum)
{
    TKIV->updateFromBacWidget();
    Q_UNUSED(bacnum)
}

QWidget *TuneKIVMain::MainUI()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *tw = new QTabWidget;
    tw->setObjectName("tunetw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: " + QString(Colors::Tab) + ";}";
    tw->tabBar()->setStyleSheet(ConfTWss);
    tw->addTab(TKIV->BacWidget(), "Настроечные параметры");
    tw->addTab(TKIV->BdaWidget(), "Текущие данные без настройки");
    lyout->addWidget(tw);
    w->setLayout(lyout);
    return w;
}

Error::Msg TuneKIVMain::showPreWarning()
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
        "3. Данный этап регулировки должен выполняться при температуре"
        "окружающего воздуха +20±7 °С. Если температура окружающего воздуха отличается от указанной,"
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

Error::Msg TuneKIVMain::checkTuneCoefs()
{
    QVector<float *> tcoefs = { &TKIV->m_Bac.KmU[0], &TKIV->m_Bac.KmI1[0], &TKIV->m_Bac.KmI2[0], &TKIV->m_Bac.KmI4[0],
        &TKIV->m_Bac.KmI8[0], &TKIV->m_Bac.KmI16[0], &TKIV->m_Bac.KmI32[0] };
    for (int i = 0; i < 3; ++i)
    {
        foreach (float *coef, tcoefs)
            if (!StdFunc::floatIsWithinLimits(*(coef + i), 1.0, 0.05))
                return Error::Msg::GeneralError;
    }
    if (!StdFunc::floatIsWithinLimits(TKIV->m_Bac.K_freq, 1.0, 0.05))
        return Error::Msg::GeneralError;
    for (int i = 0; i < 6; ++i)
    {
        if (!StdFunc::floatIsWithinLimits(TKIV->m_Bac.DPsi[i], 0.0, 1.0))
            return Error::Msg::GeneralError;
    }
    return Error::Msg::NoError;
}

Error::Msg TuneKIVMain::setR80()
{
    setR(80);
    return Error::Msg::NoError;
}

Error::Msg TuneKIVMain::processR80()
{
    m_pt100 = processR();
    if (StdFunc::isCancelled())
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}

Error::Msg TuneKIVMain::setR120()
{
    setR(120);
    return Error::Msg::NoError;
}

Error::Msg TuneKIVMain::processR120()
{
    double pt100_120 = processR();
    if (StdFunc::isCancelled())
        return Error::Msg::GeneralError;
    if (StdFunc::floatIsWithinLimits(pt100_120, m_pt100))
    {
        WARNMSG("Ошибка в полученных данных, значения сопротивлений равны");
        StdFunc::cancel();
        return Error::Msg::GeneralError;
    }
    TKIV->m_Bac.Art = (pt100_120 - m_pt100) / 40;
    TKIV->m_Bac.Brt = pt100_120 * 2 - m_pt100 * 3;
    TKIV->updateBacWidget();
    saveAllTuneCoefs();
    return Error::Msg::NoError;
}

void TuneKIVMain::setR(int r)
{
    if (QMessageBox::question(this, "Подтверждение", "Установите сопротивление " + QString::number(r, 'f', 1) + " Ом")
        == QMessageBox::No)
        CancelTune();
}

double TuneKIVMain::processR()
{
    startWait();
    int i = 0;
    double pt100 = 0.0;
    while ((!StdFunc::isCancelled()) && (i < StdFunc::N()))
    {
        iface()->reqBlockSync(1, DataTypes::DataBlockTypes::BdaBlock, &TKIV->m_Bda, sizeof(TKIV->m_Bda));
        TKIV->updateBdaWidget();
        pt100 += TKIV->m_Bda.Pt100;
        ++i;
        StdFunc::Wait(500);
    }
    if (StdFunc::isCancelled())
        return 0;
    stopWait();
    pt100 /= i;
    return pt100;
}

Error::Msg TuneKIVMain::ADCCoef1()
{
    CKIV->Bci_block.Unom = 220;
    if (setADCCoef(1) != Error::Msg::NoError)
        return Error::Msg::GeneralError;
    showRetomDialog(1);
    if (StdFunc::isCancelled())
        return Error::Msg::GeneralError;
    startWait();
    int i = 0;
    double uefnatf[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
    while ((!StdFunc::isCancelled()) && (i < StdFunc::N()))
    {
        iface()->reqBlockSync(1, DataTypes::DataBlockTypes::BdBlock, &TKIV->m_Bda_in, sizeof(TKIV->m_Bda_in));
        if (checkBdaIn())
        {
            TKIV->updateBdaInWidget();
            for (int j = 0; j < 6; ++j)
                uefnatf[j] += TKIV->m_Bda_in.IUefNat_filt[j];
        }
        ++i;
        StdFunc::Wait(500);
    }
    if (StdFunc::isCancelled())
        return Error::Msg::GeneralError;
    stopWait();
    af pijfajfew - r wp
}

Error::Msg TuneKIVMain::ADCCoef2()
{
    return setADCCoef(2);
}

Error::Msg TuneKIVMain::ADCCoef4()
{
    return setADCCoef(4);
}

Error::Msg TuneKIVMain::ADCCoef8()
{
    return setADCCoef(8);
}

Error::Msg TuneKIVMain::ADCCoef16()
{
    return setADCCoef(16);
}

Error::Msg TuneKIVMain::ADCCoef32()
{
    return setADCCoef(32);
}

Error::Msg TuneKIVMain::setADCCoef(int coef)
{
    QMap<int, int> adcCoefMap = { { 1, 9000 }, { 2, 4500 }, { 4, 2250 }, { 8, 1124 }, { 16, 562 }, { 32, 281 } };
    CKIV->Bci_block.C_pasp[0] = CKIV->Bci_block.C_pasp[1] = CKIV->Bci_block.C_pasp[2] = adcCoefMap[coef];

    return iface()->writeConfFileSync();
}

Error::Msg TuneKIVMain::showRetomDialog(int coef)
{
    struct retomStruct
    {
        double i;
        double range;
        QString ret10c;
    };

    QMap<int, retomStruct> retomCoefMap
        = { { 1, { 290, 2.5, "30:3" } }, { 2, { 250, 2.5, "30:3" } }, { 4, { 140, 1, "30:6" } },
              { 8, { 80, 0.5, "30:6" } }, { 16, { 40, 0.1, "1:1" } }, { 32, { 23, 0.05, "1:1" } } };
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(WDFunc::NewLBL(this,
        "Задайте на РЕТОМ-51 или имитаторе АВМ-КИВ трёхфазный режим токов и напряжений (Uabc, Iabc)"
        "Угол между токами и напряжениями: 89.9 град. (tg 2 % в имитаторе),\n"
        "Значения напряжений: 57.5 В, токов: "
            + QString::number(retomCoefMap[coef].i, 'f', 2) + " мА"));
    lyout->addWidget(WDFunc::NewLBL(this,
        "Значения тока и напряжения контролируются по показаниям прибора Энергомонитор.\n"
        "Предел измерения тока в Энергомониторе: "
            + QString::number(retomCoefMap[coef].range, 'f', 2)
            + " А.\n"
              "Коэффициент передачи РЕТ-10 для режима с имитатором: "
            + retomCoefMap[coef].ret10c + "(для РЕТОМ-51 коэффициент 30:3)."));
    QPushButton *pb = new QPushButton("Готово");
    connect(pb, SIGNAL(clicked()), dlg, SLOT(close()));
    lyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb, SIGNAL(clicked()), this, SLOT(CancelTune()));
    connect(pb, SIGNAL(clicked()), dlg, SLOT(close()));
    lyout->addWidget(pb);
    dlg->setLayout(lyout);
    dlg->exec();
    return Error::Msg::NoError;
}

void TuneKIVMain::showTWTab(int num)
{
    QTabWidget *tw = this->findChild<QTabWidget *>("tunetw");
    if (tw != nullptr)
        tw->setCurrentIndex(num);
}

bool TuneKIVMain::checkBdaIn()
{
    for (int i = 0; i < 3; ++i)
    {
        if (StdFunc::floatIsWithinLimits(TKIV->m_Bda_in.IUefNat_filt[i], 57.75, 3.0))
        {
            if (StdFunc::floatIsWithinLimits(TKIV->m_Bda_in.IUeff_filtered[i], 57.75, 3.0))
            {
                if (StdFunc::floatIsWithinLimits(TKIV->m_Bda_in.IUefNat_filt[i + 3], 290, 10))
                {
                    if (StdFunc::floatIsWithinLimits(TKIV->m_Bda_in.IUeff_filtered[i + 3], 290, 10))
                    {
                        if (StdFunc::floatIsWithinLimits(TKIV->m_Bda_in.phi_next_f[i], 0, 1))
                        {
                            if (StdFunc::floatIsWithinLimits(TKIV->m_Bda_in.phi_next_f[i + 3], 90, 1))
                                continue;
                        }
                    }
                }
            }
        }
        return false;
    }
    if (StdFunc::floatIsWithinLimits(TKIV->m_Bda_in.Pt100_R, 100, 5))
        return true;
    return false;
}
