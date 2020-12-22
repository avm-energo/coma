#include "tunekivtemp60.h"

#include "../gen/board.h"
#include "../gen/colors.h"
#include "../gen/stdfunc.h"
#include "../widgets/waitwidget.h"
#include "../widgets/wd_func.h"

#include <QEventLoop>
#include <QMessageBox>
#include <QVBoxLayout>

TuneKIVTemp60::TuneKIVTemp60(int tuneStep, ConfigKIV *ckiv, TuneKIV *tkiv, QWidget *parent)
    : AbstractTuneDialog(tuneStep, parent)
{
    //    m_tuneStep = 2;
    TKIV = tkiv;
    CKIV = ckiv;
    //    if (LoadTuneSequenceFile() != Error::Msg::NoError)
    //        return;
    //    SetBac(&TKIV->m_Bac, 1, sizeof(TKIV->m_Bac));
    SetBac(TKIV->m_Bac);
    addWidgetToTabWidget(TKIV->m_Bac->widget(), "Настроечные параметры");
    addWidgetToTabWidget(TKIV->m_Bdain->widget(), "Текущие данные");
    addWidgetToTabWidget(TKIV->m_Bd0->widget(), "Общие данные");
    //    AddBac(&m_Bac_block, M_BACBLOCKNUM, sizeof(m_Bac_block));
    SetupUI();
}

void TuneKIVTemp60::setMessages()
{
    m_messages.append("1. Ввод пароля...");
    m_messages.append("2. Сохранение конфигурации...");
    m_messages.append("3. Задание временной конфигурации и настроечных параметров...");
    m_messages.append("4. Выдача диалога о температуре в камере...");
    m_messages.append("5. Ожидание установления температурного режима...");
    m_messages.append("6. Диалог об установлении входных сигналов...");
    m_messages.append("7. Измерения...");
    if (m_tuneStep == 3)
        m_messages.append("8. Ввод данных энергомонитора и сохранение промежуточных данных...");
    else
        m_messages.append("8. Ввод данных энергомонитора и запись коэффициентов в модуль...");
}

void TuneKIVTemp60::setTuneFunctions()
{
    int count = 0;
    m_tuneFunctions[m_messages.at(count++)]
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::CheckPassword);
    Error::Msg (AbstractTuneDialog::*func)()
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::saveWorkConfig);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVTemp60::setNewConfAndTune);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVTemp60::showTempDialog);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVTemp60::waitForTempToRise);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVTemp60::showSignalsDialog);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVTemp60::analogMeasurement);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVTemp60::inputEnergomonitorValues);
    m_tuneFunctions[m_messages.at(count++)] = func;
}

// void TuneKIVTemp60::FillBac(int bacnum)
//{
//    Q_UNUSED(bacnum);
//    TKIV->updateBacWidget();
//}

// void TuneKIVTemp60::FillBackBac(int bacnum)
//{
//    TKIV->updateFromBacWidget();
//    Q_UNUSED(bacnum);
//}

// QWidget *TuneKIVTemp60::MainUI()
//{
//    QWidget *w = new QWidget;
//    QVBoxLayout *lyout = new QVBoxLayout;
//    QTabWidget *tw = new QTabWidget;
//    tw->setObjectName("tunetw");
//    QString ConfTWss = "QTabBar::tab:selected {background-color: " + QString(Colors::Tab) + ";}";
//    tw->tabBar()->setStyleSheet(ConfTWss);
//    tw->addTab(TKIV->m_Bac->widget(), "Настроечные параметры");
//    tw->addTab(TKIV->BdaWidget(), "Текущие данные");
//    tw->addTab(TKIV->m_Bd0->widget(), "Общие данные");
//    lyout->addWidget(tw);
//    w->setLayout(lyout);
//    return w;
//}

Error::Msg TuneKIVTemp60::setNewConfAndTune()
{
    CKIV->Bci_block.C_pasp[0] = CKIV->Bci_block.C_pasp[1] = CKIV->Bci_block.C_pasp[2] = 2250;
    CKIV->Bci_block.Unom = 220;
    if (BaseInterface::iface()->writeConfFileSync() != Error::Msg::NoError)
        return Error::Msg::GeneralError;
    for (int i = 0; i < 6; ++i)
    {
        TKIV->m_Bac->data()->TKUa[i] = 0;
        TKIV->m_Bac->data()->TKUb[i] = 0;
    }
    TKIV->m_Bac->data()->TKPsi_a[0] = TKIV->m_Bac->data()->TKPsi_a[1] = TKIV->m_Bac->data()->TKPsi_a[2] = 0;
    TKIV->m_Bac->data()->TKPsi_b[0] = TKIV->m_Bac->data()->TKPsi_b[1] = TKIV->m_Bac->data()->TKPsi_a[2] = 0;
    if (!writeTuneCoefs())
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}

Error::Msg TuneKIVTemp60::showTempDialog()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;

    QString tempstr = (m_tuneStep == TuneKIV::TS_60TUNING) ? "+60" : "-20";
    lyout->addWidget(
        WDFunc::NewLBL2(this, "Поместите модуль в термокамеру, установите температуру " + tempstr + " ± 2 °С"));
    lyout->addWidget(WDFunc::NewPB(this, "", "Готово", [dlg] { dlg->close(); }));
    lyout->addWidget(WDFunc::NewPB(this, "cancelpb", "Отмена", [dlg] { dlg->close(); }));
    dlg->setLayout(lyout);
    WDFunc::PBConnect(dlg, "cancelpb", static_cast<AbstractTuneDialog *>(this), &AbstractTuneDialog::CancelTune);
    dlg->exec();
    return Error::Msg::NoError;
}

Error::Msg TuneKIVTemp60::waitForTempToRise()
{
    WaitWidget *ww = new WaitWidget;
    ww->setObjectName("ww");
    WaitWidget::ww_struct wws = { true, false, WaitWidget::WW_TIME,
        1800 }; // isallowedtostop = true, isIncrement = false, format: mm:ss, 30 minutes
    ww->Init(wws);
    ww->SetMessage("Пожалуйста, подождите");
    QEventLoop loop;
    connect(ww, &WaitWidget::finished, &loop, &QEventLoop::quit);
    ww->Start();
    loop.exec();
    if (StdFunc::isCancelled())
        return Error::Msg::ResEmpty;
    return Error::Msg::NoError;
}

Error::Msg TuneKIVTemp60::showSignalsDialog()
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
        "3. Задайте на РЕТОМ-51 или имитаторе АВМ-КИВ трёхфазный режим токов и напряжений (Uabc, Iabc)"
        "Угол между токами и напряжениями: 89.9 град. (tg 2 % в имитаторе),\n"
        "Значения напряжений: 57.5 В, токов: 140 мА"));
    lyout->addWidget(WDFunc::NewPB(this, "", "Готово", [dlg] { dlg->close(); }));
    lyout->addWidget(WDFunc::NewPB(this, "cancelpb", "Отмена", [dlg] { dlg->close(); }));
    dlg->setLayout(lyout);
    WDFunc::PBConnect(dlg, "cancelpb", static_cast<AbstractTuneDialog *>(this), &AbstractTuneDialog::CancelTune);
    dlg->exec();
    return Error::Msg::NoError;
}

Error::Msg TuneKIVTemp60::analogMeasurement()
{
    emit setProgressSize(StdFunc::tuneRequestCount());
    //    startWait();
    int i = 0;
    for (int i = 0; i < 6; ++i)
    {
        m_midTuneStruct.u[i] = 0.0;
        if (i < 3)
            m_midTuneStruct.y[i] = 0.0;
    }
    m_midTuneStruct.tmk = 0.0;
    while ((!StdFunc::isCancelled()) && (i < StdFunc::tuneRequestCount()))
    {
        //        BaseInterface::iface()->reqBlockSync(
        //            1, DataTypes::DataBlockTypes::BdBlock, &TKIV->m_Bda_in, sizeof(TKIV->m_Bda_in));
        //        BaseInterface::iface()->reqBlockSync(
        //            0, DataTypes::DataBlockTypes::BdBlock, TKIV->m_Bd0->data(), sizeof(Bd0::BlockData));
        //        TKIV->updateBdaInWidget();
        //        //        TKIV->updateBd0Widget();
        //        TKIV->m_Bd0->updateWidget();
        TKIV->m_Bd0->readAndUpdate();
        TKIV->m_Bdain->readAndUpdate();
        for (int j = 0; j < 6; ++j)
        {
            m_midTuneStruct.u[j] += TKIV->m_Bdain->data()->IUefNat_filt[j];
            if (j < 3)
                m_midTuneStruct.y[j] += TKIV->m_Bdain->data()->phi_next_f[j + 3];
        }
        //        m_midTuneStruct.tmk += TKIV->m_Bd0.Tmk;
        m_midTuneStruct.tmk += TKIV->m_Bd0->data()->Tmk;
        ++i;
        emit setProgressCount(i);
        StdFunc::Wait(500);
    }
    for (int i = 0; i < 6; ++i)
    {
        m_midTuneStruct.u[i] /= StdFunc::tuneRequestCount();
        if (i < 3)
            m_midTuneStruct.y[i] /= StdFunc::tuneRequestCount();
    }
    m_midTuneStruct.tmk /= StdFunc::tuneRequestCount();
    if (StdFunc::isCancelled())
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}

Error::Msg TuneKIVTemp60::inputEnergomonitorValues()
{
    QDialog *dlg = new QDialog(this);
    dlg->setObjectName("energomonitordlg");
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addWidget(WDFunc::NewLBL2(this, "Ввод значений сигналов c Энергомонитора"));

    vlyout->addWidget(WDFunc::NewLBLAndLE(this, "Uэт", "ValuetuneU", true));
    vlyout->addWidget(WDFunc::NewLBLAndLE(this, "Iэт", "ValuetuneI", true));
    vlyout->addWidget(WDFunc::NewLBLAndLE(this, "Yэт", "ValuetuneY", true));
    vlyout->addWidget(WDFunc::NewLBLAndLE(this, "fэт:", "ValuetuneF", true));
    QPushButton *pb = new QPushButton("Настроить");
    if (m_tuneStep == TuneKIV::TS_60TUNING)
        connect(pb, &QPushButton::clicked, this, &TuneKIVTemp60::saveIntermediateResults);
    else
        connect(pb, &QPushButton::clicked, this, &TuneKIVTemp60::calcTuneCoefsAndWrite);
    vlyout->addWidget(pb);
    dlg->setLayout(vlyout);
    dlg->exec();
    return Error::Msg::NoError;
}

Error::Msg TuneKIVTemp60::calcTuneCoefsAndWrite()
{
    MidTuneStruct tunenegative = m_midTuneStruct;

    loadWorkConfig();
    loadIntermediateResults();
    MidTuneStruct tunepositive = m_midTuneStruct;

    float dTp, dTm;

    dTp = tunepositive.tmk - TKIV->m_Bac->data()->Tmk0;
    dTm = tunenegative.tmk - TKIV->m_Bac->data()->Tmk0;

    for (int i = 0; i < 6; i++)
    {
        float dUp = (tunepositive.u[i] / tunepositive.uet) - 1;
        float dUm = (tunenegative.u[i] / tunenegative.uet) - 1;
        TKIV->m_Bac->data()->TKUa[i] = ((dUm * dTp * dTp) - (dUp * dTm * dTm)) / (dTp * dTm * (dTp - dTm));
        TKIV->m_Bac->data()->TKUb[i] = ((dUp * dTm) - (dUm * dTp)) / (dTp * dTm * (dTp - dTm));
    }

    for (int i = 0; i < 3; i++)
    {
        float dYp = (tunepositive.y[i] - tunepositive.yet);
        float dYm = (tunenegative.y[i] - tunenegative.yet);
        TKIV->m_Bac->data()->TKPsi_a[i] = ((dYm * dTp * dTp) - (dYp * dTm * dTm)) / (dTp * dTm * (dTp - dTm));
        TKIV->m_Bac->data()->TKPsi_b[i] = ((dYp * dTm) - (dYm * dTp)) / (dTp * dTm * (dTp - dTm));
    }
    //    TKIV->updateBacWidget();
    TKIV->m_Bac->updateWidget();
    if (showTuneCoefs() != Error::Msg::NoError)
        return Error::Msg::GeneralError;
    TKIV->m_Bac->writeBlockToModule();
    return Error::Msg::NoError;
}

void TuneKIVTemp60::loadIntermediateResults()
{
    QString cpuserialnum = Board::GetInstance().UID();
    QSettings storedcalibrations(StdFunc::GetSystemHomeDir() + "calibr.ini", QSettings::IniFormat);
    foreach (TuneDescrStruct item, m_tuneDescrVector())
        *item.parameter
            = StdFunc::toFloat(storedcalibrations.value(cpuserialnum + "/" + item.parametername).toString());
}

Error::Msg TuneKIVTemp60::showTuneCoefs()
{
    Bac *newbac = new Bac;
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;
    dlg->setWindowTitle("Проверка коэффициентов");
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setObjectName("showtunedlg");
    lyout->addWidget(newbac->widget());
    lyout->addWidget(WDFunc::NewPB(this, "acceptpb", "Записать в модуль", this, &TuneKIVTemp60::acceptTuneCoefs));
    lyout->addWidget(WDFunc::NewPB(
        this, "cancelpb", "Отменить", static_cast<AbstractTuneDialog *>(this), &AbstractTuneDialog::CancelTune));
    dlg->setLayout(lyout);
    memcpy(newbac->data(), TKIV->m_Bac->data(), sizeof(Bac::BlockData));
    newbac->updateWidget();
    dlg->show();
    QEventLoop loop;
    connect(this, &TuneKIVTemp60::closeShowTuneDialog, &loop, &QEventLoop::quit);
    loop.exec();
    dlg->close();
    return (StdFunc::isCancelled()) ? Error::Msg::GeneralError : Error::Msg::NoError;
}

void TuneKIVTemp60::saveIntermediateResults()
{
    QString cpuserialnum = Board::GetInstance().UID();
    QSettings storedcalibrations(StdFunc::GetSystemHomeDir() + "calibr.ini", QSettings::IniFormat);
    foreach (TuneDescrStruct item, m_tuneDescrVector())
        storedcalibrations.setValue(cpuserialnum + "/" + item.parametername, *item.parameter);
    loadWorkConfig();
}

void TuneKIVTemp60::acceptTuneCoefs()
{
    TKIV->m_Bac->updateFromWidget();
    TKIV->m_Bac->writeBlockToModule();
}

// void TuneKIVTemp60::setDefCoefs()
//{
//    TKIV->m_Bac->setDefBlockAndUpdate();
//    TKIV->m_Bd0->setDefBlockAndUpdate();
//}
