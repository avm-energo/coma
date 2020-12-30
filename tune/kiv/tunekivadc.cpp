#include "tunekivadc.h"

#include "../../gen/colors.h"
#include "../../gen/stdfunc.h"
#include "../../widgets/waitwidget.h"
#include "../../widgets/wd_func.h"

#include <QMessageBox>
#include <QVBoxLayout>

TuneKIVADC::TuneKIVADC(int tuneStep, ConfigKIV *ckiv, QWidget *parent) : AbstractTuneDialog(tuneStep, parent)
{
    CKIV = ckiv;
    //    TKIV = kiv;
    //    m_tuneStep = 1;
    //    SetBac(m_bac, 1, sizeof(m_bac));
    m_bac = new Bac;
    m_bda = new Bda;
    m_bdain = new BdaIn;
    m_bd0 = new Bd0;
    SetBac(m_bac);
    m_BacWidgetIndex = addWidgetToTabWidget(m_bac->widget(), "Настроечные параметры");
    m_BdainWidgetIndex = addWidgetToTabWidget(m_bdain->widget(), "Текущие данные");
    addWidgetToTabWidget(m_bd0->widget(), "Общие данные");
    //    SetupUI();
    m_isEnergoMonitorDialogCreated = false;
    SetupUI();
}

void TuneKIVADC::setMessages()
{
    m_messages.append("1. Ввод пароля...");
    m_messages.append("2. Отображение предупреждения...");
    m_messages.append("3. Запрос настроечных параметров...");
    m_messages.append("4. Задание режима конфигурирования модуля...");
    m_messages.append("5. Регулировка для Кацп = 1...");
    m_messages.append("6. Отображение диалога задания входных данных...");
    m_messages.append("7. Регулировка для Кацп = 2...");
    m_messages.append("8. Отображение диалога задания входных данных...");
    m_messages.append("9. Регулировка для Кацп = 4...");
    m_messages.append("10. Отображение диалога задания входных данных...");
    m_messages.append("11. Регулировка для Кацп = 8...");
    m_messages.append("12. Отображение диалога задания входных данных...");
    m_messages.append("13. Регулировка для Кацп = 16...");
    m_messages.append("14. Отображение диалога задания входных данных...");
    m_messages.append("15. Регулировка для Кацп = 32...");
    m_messages.append("16. Отображение диалога задания входных данных...");
    m_messages.append("17. Регулировка канала Tmk0...");
    m_messages.append("18. Запись настроечных коэффициентов и восстановление конфигурации...");
    m_messages.append("19. Проверка регулировки...");
}

void TuneKIVADC::setTuneFunctions()
{
    int count = 0;
    m_tuneFunctions[m_messages.at(count++)]
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::CheckPassword);
    Error::Msg (AbstractTuneDialog::*func)()
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVADC::showPreWarning);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::readTuneCoefs);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVADC::setSMode2);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVADC::ADCCoef1);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVADC::showEnergomonitorInputDialog);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVADC::ADCCoef2);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVADC::showEnergomonitorInputDialog);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVADC::ADCCoef4);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVADC::showEnergomonitorInputDialog);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVADC::ADCCoef8);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVADC::showEnergomonitorInputDialog);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVADC::ADCCoef16);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVADC::showEnergomonitorInputDialog);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVADC::ADCCoef32);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVADC::showEnergomonitorInputDialog);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVADC::Tmk0);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVADC::SendBac);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVADC::CheckTune);
    m_tuneFunctions[m_messages.at(count++)] = func;
}

// void TuneKIVADC::FillBac(int bacnum)
//{
//    TKIV->updateBacWidget();
//    Q_UNUSED(bacnum)
//}

// void TuneKIVADC::FillBackBac(int bacnum)
//{
//    TKIV->updateFromBacWidget();
//    Q_UNUSED(bacnum)
//}

// QWidget *TuneKIVADC::MainUI()
//{
//    QWidget *w = new QWidget;
//    QVBoxLayout *lyout = new QVBoxLayout;
//    QTabWidget *tw = new QTabWidget;
//    tw->setObjectName("tunetw");
//    QString ConfTWss = "QTabBar::tab:selected {background-color: " + QString(Colors::Tab) + ";}";
//    tw->tabBar()->setStyleSheet(ConfTWss);
//    tw->addTab(m_bac->widget(), "Настроечные параметры");
//    tw->addTab(TKIV->BdaWidget(), "Текущие данные");
//    tw->addTab(m_bd0->widget(), "Общие данные");
//    lyout->addWidget(tw);
//    w->setLayout(lyout);
//    return w;
//}

Error::Msg TuneKIVADC::showPreWarning()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;

    lyout->addWidget(WDFunc::NewLBL2(this, "", "", new QPixmap("images/tunekiv1.png")));
    lyout->addWidget(WDFunc::NewLBL2(this, "1. Соберите схему подключения по одной из вышеприведённых картинок;"));
    lyout->addWidget(WDFunc::NewLBL2(this,
        "2. Включите питание Энергомонитор 3.1КМ и настройте его на режим измерения тока"
        "и напряжения в однофазной сети переменного тока, установите предел измерения"
        "по напряжению 60 В, по току - 2,5 А;"));
    lyout->addWidget(WDFunc::NewLBL2(this,
        "3. Данный этап регулировки должен выполняться при температуре"
        "окружающего воздуха +20±7 °С. Если температура окружающего воздуха отличается от указанной,"
        "разместите модуль в термокамеру с диапазоном регулирования температуры "
        "от минус 20 до +60°С. Установите нормальное значение температуры "
        "в камере 20±5°С"));
    lyout->addWidget(WDFunc::NewPB(this, "", "Готово", [dlg] { dlg->close(); }));
    lyout->addWidget(WDFunc::NewPB(this, "cancelpb", "Отмена", [dlg] { dlg->close(); }));
    dlg->setLayout(lyout);
    WDFunc::PBConnect(dlg, "cancelpb", static_cast<AbstractTuneDialog *>(this), &AbstractTuneDialog::CancelTune);
    dlg->exec();
    return Error::Msg::NoError;
}

Error::Msg TuneKIVADC::setSMode2()
{
    BaseInterface::iface()->writeCommand(Queries::QUSB_SetMode, 0x02);
    return Error::Msg::NoError;
}

Error::Msg TuneKIVADC::ADCCoef(int coef)
{
    QMap<int, int> currentMap = { { 1, 290 }, { 2, 250 }, { 4, 140 }, { 8, 80 }, { 16, 40 }, { 32, 23 } };
    m_curTuneStep = coef;
    CKIV->Bci_block.Unom = 220;
    Error::Msg res = setADCCoef(coef);
    if (res != Error::Msg::NoError)
        return res;
    showRetomDialog(coef);
    if (StdFunc::isCancelled())
        return Error::Msg::GeneralError;
    showTWTab(m_BdainWidgetIndex);
    emit setProgressSize(StdFunc::tuneRequestCount());
    //    startWait();
    int i = 0;
    for (int i = 0; i < 6; ++i)
    {
        m_bdainBlockData.IUefNat_filt[i] = 0.0;
        m_bdainBlockData.phi_next_f[i] = 0.0;
    }
    while ((!StdFunc::isCancelled()) && (i < StdFunc::tuneRequestCount()))
    {
        m_bdain->readAndUpdate();
        //        BaseInterface::iface()->reqBlockSync(
        //            1, DataTypes::DataBlockTypes::BdBlock, &TKIV->m_Bda_in, sizeof(TKIV->m_Bda_in));
        if (checkBdaIn(currentMap[coef]))
        {
            //            TKIV->updateBdaInWidget();
            for (int j = 0; j < 6; ++j)
            {
                m_bdainBlockData.IUefNat_filt[j] += m_bdain->data()->IUefNat_filt[j];
                m_bdainBlockData.phi_next_f[j] += m_bdain->data()->phi_next_f[j];
            }
        }
        else
            return Error::Msg::GeneralError;
        ++i;
        emit setProgressCount(i);
        StdFunc::Wait(500);
    }
    for (int i = 0; i < 6; ++i)
    {
        m_bdainBlockData.IUefNat_filt[i] /= StdFunc::tuneRequestCount();
        m_bdainBlockData.phi_next_f[i] /= StdFunc::tuneRequestCount();
    }
    if (StdFunc::isCancelled())
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
    //    stopWait();
}

Error::Msg TuneKIVADC::ADCCoef1()
{
    return ADCCoef(1);
}

Error::Msg TuneKIVADC::ADCCoef2()
{
    return ADCCoef(2);
}

Error::Msg TuneKIVADC::ADCCoef4()
{
    return ADCCoef(4);
}

Error::Msg TuneKIVADC::ADCCoef8()
{
    return ADCCoef(8);
}

Error::Msg TuneKIVADC::ADCCoef16()
{
    return ADCCoef(16);
}

Error::Msg TuneKIVADC::ADCCoef32()
{
    return ADCCoef(32);
}

Error::Msg TuneKIVADC::Tmk0()
{
    emit setProgressSize(5);
    //    startWait();
    int i = 0;
    double tmk0 = 0;
    while ((!StdFunc::isCancelled()) && (i < 5))
    {
        //        BaseInterface::iface()->reqBlockSync(0, DataTypes::DataBlockTypes::BdBlock, &m_bd0,
        //        sizeof(m_bd0));
        //        BaseInterface::iface()->reqBlockSync(
        //            0, DataTypes::DataBlockTypes::BdBlock, m_bd0->data(), sizeof(Bd0::BlockData));
        //        m_bd0->updateWidget();
        m_bd0->readAndUpdate();
        //        TKIV->updateBd0Widget();
        tmk0 += m_bd0->data()->Tmk;
        ++i;
        emit setProgressCount(i);
        StdFunc::Wait(500);
    }
    if (StdFunc::isCancelled())
        return Error::Msg::GeneralError;
    m_bac->data()->Tmk0 = tmk0 / 5;
    return Error::Msg::NoError;
}

Error::Msg TuneKIVADC::SendBac()
{
    if (!writeTuneCoefs())
        return Error::Msg::GeneralError;
    if (!loadWorkConfig())
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}

Error::Msg TuneKIVADC::CheckTune()
{
    QMessageBox::information(
        this, "Информация", "Для завершения настройки нажмите Enter\nДля отказа от настройки нажмите Esc");
    m_finished = false;
    while ((!StdFunc::isCancelled()) && !m_finished)
    {
        //        BaseInterface::iface()->reqBlockSync(
        //            1, DataTypes::DataBlockTypes::BdBlock, &TKIV->m_Bda_in, sizeof(TKIV->m_Bda_in));
        //        TKIV->updateBdaInWidget();
        m_bdain->readAndUpdate();
        StdFunc::Wait(500);
    }
    if (StdFunc::isCancelled())
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}

Error::Msg TuneKIVADC::setADCCoef(int coef)
{
    QMap<int, int> adcCoefMap = { { 1, 9000 }, { 2, 4500 }, { 4, 2250 }, { 8, 1124 }, { 16, 562 }, { 32, 281 } };
    CKIV->Bci_block.C_pasp[0] = CKIV->Bci_block.C_pasp[1] = CKIV->Bci_block.C_pasp[2] = adcCoefMap[coef];

    return BaseInterface::iface()->writeConfFileSync();
}

Error::Msg TuneKIVADC::showRetomDialog(int coef)
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
    lyout->addWidget(WDFunc::NewLBL2(this,
        "Задайте на РЕТОМ-51 или имитаторе АВМ-КИВ трёхфазный режим токов и напряжений (Uabc, Iabc)"
        "Угол между токами и напряжениями: 89.9 град. (tg 2 % в имитаторе),\n"
        "Значения напряжений: 57.5 В, токов: "
            + QString::number(retomCoefMap[coef].i, 'f', 2) + " мА"));
    lyout->addWidget(WDFunc::NewLBL2(this,
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

// void TuneKIVADC::showTWTab(int num)
//{
//    QTabWidget *tw = this->findChild<QTabWidget *>("tunetw");
//    if (tw != nullptr)
//        tw->setCurrentIndex(num);
//}

bool TuneKIVADC::checkBdaIn(int current)
{
    for (int i = 0; i < 3; ++i)
    {
        if (StdFunc::floatIsWithinLimits(this, m_bdain->data()->IUefNat_filt[i], 60.0, 3.0))
        {
            if (StdFunc::floatIsWithinLimits(this, m_bdain->data()->IUeff_filtered[i], 60.0, 3.0))
            {
                if (StdFunc::floatIsWithinLimits(this, m_bdain->data()->IUefNat_filt[i + 3], current, 10))
                {
                    if (StdFunc::floatIsWithinLimits(this, m_bdain->data()->IUeff_filtered[i + 3], current, 10))
                    {
                        if (StdFunc::floatIsWithinLimits(this, m_bdain->data()->phi_next_f[i], 0, 1))
                        {
                            if (StdFunc::floatIsWithinLimits(this, m_bdain->data()->phi_next_f[i + 3], 89, 1))
                                continue;
                        }
                    }
                }
            }
        }
        return false;
    }
    if (StdFunc::floatIsWithinLimits(this, m_bdain->data()->Pt100_R, 100, 5))
        return true;
    return false;
}

Error::Msg TuneKIVADC::showEnergomonitorInputDialog()
{
    if (!m_isEnergoMonitorDialogCreated)
    {
        QDialog *dlg = new QDialog(this);
        // QVBoxLayout *lyout = new QVBoxLayout;
        //        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->setObjectName("energomonitordlg");
        QVBoxLayout *vlyout = new QVBoxLayout;
        vlyout->addWidget(WDFunc::NewLBL2(this, "Ввод значений сигналов c Энергомонитора"));

        vlyout->addWidget(WDFunc::NewLBLAndLE(this, "Uэт", "ValuetuneU", true));
        vlyout->addWidget(WDFunc::NewLBLAndLE(this, "Iэт", "ValuetuneI", true));
        vlyout->addWidget(WDFunc::NewLBLAndLE(this, "Yэт", "ValuetuneY", true));

        //        for (i = 0; i < 3; i++)
        //        {
        //            lbl = new QLabel("Iэт[" + QString::number(i) + "]:");
        //            glyout->addWidget(lbl, 3, i, 1, 1);
        //            ledit = new QLineEdit();
        //            ledit->setObjectName("ValuetuneI" + QString::number(i));
        //            glyout->addWidget(ledit, 4, i, 1, 1);
        //        }

        //        for (i = 0; i < 6; i++)
        //        {
        //            lbl = new QLabel("φэт[" + QString::number(i) + "]:");
        //            glyout->addWidget(lbl, 5, i, 1, 1);
        //            ledit = new QLineEdit();
        //            ledit->setObjectName("ValuetunePhi" + QString::number(i));
        //            glyout->addWidget(ledit, 6, i, 1, 1);
        //        }

        vlyout->addWidget(WDFunc::NewLBLAndLE(this, "fэт:", "ValuetuneF", true));
        //        glyout->addWidget(WDFunc::NewLE(this, "ValuetuneU" + QString::number(i)), 2, i, 1, 1);
        //        lbl = new QLabel("fэт:");
        //        glyout->addWidget(lbl, 7, 0, 1, 1);
        //        ledit = new QLineEdit();
        //        ledit->setObjectName("ValuetuneF");
        //        glyout->addWidget(ledit, 8, 0, 1, 1);

        QPushButton *pb = new QPushButton("Настроить");
        connect(pb, SIGNAL(clicked()), this, SLOT(CalcTuneCoefs()));
        vlyout->addWidget(pb);
        //     pb = new QPushButton("Отмена");
        //     connect(pb,SIGNAL(clicked()),this,SLOT(CancelTune()));
        //     connect(pb,SIGNAL(clicked()),this,SLOT(close()));
        //     connect(pb,SIGNAL(clicked()),this,SLOT(CloseAsk()));
        //     glyout->addWidget(pb,9,3,1,3);

        dlg->setLayout(vlyout);
        m_isEnergoMonitorDialogCreated = true;
        dlg->exec();

        //    lyout->addWidget(pb);
        //    dlg->setLayout(lyout);
        //    dlg->exec();
    }
    else
    {
        QDialog *dlg = this->findChild<QDialog *>("energomonitordlg");
        if (dlg != nullptr)
            dlg->exec();
    }
    bool enabled = (m_curTuneStep > 1) ? false : true;
    for (int i = 0; i < 3; ++i)
    {
        WDFunc::SetVisible(this, "ValuetuneU" + QString::number(i), enabled);
        WDFunc::SetVisible(this, "ValuetuneY" + QString::number(i), enabled);
    }
    return Error::Msg::NoError;
}

// void TuneKIVADC::saveIntermediateResults()
//{
//    struct tunedescrstruct
//    {
//        QString parametername;
//        float *parameter;
//    };

//    QVector<tunedescrstruct> tuneDescrVector;
//    for (int i = 0; i < 6; ++i)
//        tuneDescrVector.append({ "u_p" + QString::number(i), &m_bdain.IUefNat_filt[i] });
//    for (int i = 0; i < 3; ++i)
//        tuneDescrVector.append({ "y_p" + QString::number(i), &m_bdain.phi_next_f[i] });
//    tuneDescrVector.append({ "tmk_p", &m_midTuneStruct.tmk });
//    tuneDescrVector.append({ "uet_p", &m_midTuneStruct.uet });
//    tuneDescrVector.append({ "iet_p", &m_midTuneStruct.iet });
//    tuneDescrVector.append({ "yet_p", &m_midTuneStruct.yet });
//    QString cpuserialnum = Board::GetInstance().UID();
//    QSettings storedcalibrations(StdFunc::GetSystemHomeDir() + "calibr.ini", QSettings::IniFormat);
//    foreach (tunedescrstruct item, tuneDescrVector)
//        storedcalibrations.setValue(cpuserialnum + "/" + item.parametername, *item.parameter);
//    loadWorkConfig();
//}

void TuneKIVADC::CalcTuneCoefs()
{
    QMap<int, float *> kmimap = { { 2, &m_bac->data()->KmI2[0] }, { 4, &m_bac->data()->KmI4[0] },
        { 8, &m_bac->data()->KmI8[0] }, { 16, &m_bac->data()->KmI16[0] }, { 32, &m_bac->data()->KmI32[0] } };
    float uet, iet, yet, fet;
    uet = StdFunc::toFloat(WDFunc::LEData(this, "ValuetuneU"));
    iet = StdFunc::toFloat(WDFunc::LEData(this, "ValuetuneI"));
    yet = StdFunc::toFloat(WDFunc::LEData(this, "ValuetuneY"));
    fet = StdFunc::toFloat(WDFunc::LEData(this, "ValuetuneF"));
    switch (m_curTuneStep)
    {
    case 1:
    {
        for (int i = 0; i < 3; ++i)
        {
            m_bac->data()->KmU[i] = m_bac->data()->KmU[i] * uet / m_bdainBlockData.IUefNat_filt[i];
            m_bac->data()->KmI1[i] = m_bac->data()->KmI1[i] * iet / m_bdainBlockData.IUefNat_filt[i + 3];
        }
        m_bac->data()->K_freq = m_bac->data()->K_freq * fet / m_bdainBlockData.Frequency;
        for (int i = 1; i < 3; ++i)
            m_bac->data()->DPsi[i] = m_bac->data()->DPsi[i] - m_bdainBlockData.phi_next_f[i];
        for (int i = 3; i < 6; ++i)
            m_bac->data()->DPsi[i] = m_bac->data()->DPsi[i] + yet - m_bdainBlockData.phi_next_f[i];
        break;
    }
    default:
    {
        assert(kmimap.contains(m_curTuneStep));
        for (int i = 0; i < 3; ++i)
            *kmimap.value(m_curTuneStep) = *kmimap.value(m_curTuneStep) * iet / m_bdainBlockData.IUefNat_filt[i + 3];
        break;
    }
    }
    QDialog *dlg = this->findChild<QDialog *>("energomonitordlg");
    if (dlg != nullptr)
        dlg->close();
}

// void TuneKIVADC::setDefCoefs()
//{
//    m_bac->setDefBlockAndUpdate();
//}
