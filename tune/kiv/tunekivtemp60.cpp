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
    SetBac(&TKIV->m_Bac, 1, sizeof(TKIV->m_Bac));
    //    AddBac(&m_Bac_block, M_BACBLOCKNUM, sizeof(m_Bac_block));
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
    {
        m_messages.append("8. Ввод данных энергомонитора и расчёт коэффициентов...");
        m_messages.append("9. Запись коэффициентов в модуль...");
    }
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
    if (m_tuneStep == 4)
    {
        func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVTemp60::calcTuneCoefsAndWrite);
        m_tuneFunctions[m_messages.at(count++)] = func;
    }
}

void TuneKIVTemp60::FillBac(int bacnum)
{
    Q_UNUSED(bacnum);
    TKIV->updateBacWidget();
}

void TuneKIVTemp60::FillBackBac(int bacnum)
{
    TKIV->updateFromBacWidget();
    Q_UNUSED(bacnum);
}

QWidget *TuneKIVTemp60::MainUI()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *tw = new QTabWidget;
    tw->setObjectName("tunetw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: " + QString(Colors::Tab) + ";}";
    tw->tabBar()->setStyleSheet(ConfTWss);
    tw->addTab(TKIV->BacWidget(), "Настроечные параметры");
    tw->addTab(TKIV->BdaWidget(), "Текущие данные");
    tw->addTab(TKIV->BdaTempWidget(), "Блок Bda_temp");
    lyout->addWidget(tw);
    w->setLayout(lyout);
    return w;
}

Error::Msg TuneKIVTemp60::setNewConfAndTune()
{
    CKIV->Bci_block.C_pasp[0] = CKIV->Bci_block.C_pasp[1] = CKIV->Bci_block.C_pasp[2] = 2250;
    CKIV->Bci_block.Unom = 220;
    if (iface()->writeConfFileSync() != Error::Msg::NoError)
        return Error::Msg::GeneralError;
    for (int i = 0; i < 6; ++i)
    {
        TKIV->m_Bac.TKUa[i] = 0;
        TKIV->m_Bac.TKUb[i] = 0;
    }
    TKIV->m_Bac.TKPsi_a[0] = TKIV->m_Bac.TKPsi_a[1] = TKIV->m_Bac.TKPsi_a[2] = 0;
    TKIV->m_Bac.TKPsi_b[0] = TKIV->m_Bac.TKPsi_b[1] = TKIV->m_Bac.TKPsi_a[2] = 0;
    if (!writeTuneCoefs())
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}

Error::Msg TuneKIVTemp60::showTempDialog()
{
    QDialog *dlg = new QDialog;
    QVBoxLayout *lyout = new QVBoxLayout;

    lyout->addWidget(WDFunc::NewLBL(this, "Поместите модуль в термокамеру, установите температуру +60 ± 2 °С"));
    lyout->addWidget(WDFunc::NewPB2(this, "", "Готово", [dlg] { dlg->close(); }));
    lyout->addWidget(WDFunc::NewPB2(this, "cancelpb", "Отмена", [dlg] { dlg->close(); }));
    dlg->setLayout(lyout);
    WDFunc::PBConnect(dlg, "cancelpb", this, &AbstractTuneDialog::CancelTune);
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
    connect(ww, &WaitWidget::finished, &loop, &QEventLoop::exit);
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

    lyout->addWidget(WDFunc::NewLBL(this, "", "", "", new QPixmap("images/tunekiv1.png")));
    lyout->addWidget(WDFunc::NewLBL(this, "1. Соберите схему подключения по одной из вышеприведённых картинок;"));
    lyout->addWidget(WDFunc::NewLBL(this,
        "2. Включите питание Энергомонитор 3.1КМ и настройте его на режим измерения тока"
        "и напряжения в однофазной сети переменного тока, установите предел измерения"
        "по напряжению 60 В, по току - 2,5 А;"));
    lyout->addWidget(WDFunc::NewLBL(this,
        "3. Задайте на РЕТОМ-51 или имитаторе АВМ-КИВ трёхфазный режим токов и напряжений (Uabc, Iabc)"
        "Угол между токами и напряжениями: 89.9 град. (tg 2 % в имитаторе),\n"
        "Значения напряжений: 57.5 В, токов: 140 мА"));
    lyout->addWidget(WDFunc::NewPB2(this, "", "Готово", [dlg] { dlg->close(); }));
    lyout->addWidget(WDFunc::NewPB2(this, "cancelpb", "Отмена", [dlg] { dlg->close(); }));
    dlg->setLayout(lyout);
    WDFunc::PBConnect(dlg, "cancelpb", this, &AbstractTuneDialog::CancelTune);
    dlg->exec();
    return Error::Msg::NoError;
}

Error::Msg TuneKIVTemp60::analogMeasurement()
{
    emit setGeneralProgressBarSize(StdFunc::tuneRequestCount());
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
        iface()->reqBlockSync(1, DataTypes::DataBlockTypes::BdBlock, &TKIV->m_Bda_in, sizeof(TKIV->m_Bda_in));
        iface()->reqBlockSync(8, DataTypes::DataBlockTypes::BdBlock, &TKIV->m_Bda_temp, sizeof(TKIV->m_Bda_temp));
        TKIV->updateBdaInWidget();
        TKIV->updateBdaTempWidget();
        for (int j = 0; j < 6; ++j)
        {
            m_midTuneStruct.u[j] += TKIV->m_Bda_in.IUefNat_filt[j];
            if (j < 3)
                m_midTuneStruct.y[j] += TKIV->m_Bda_in.phi_next_f[j + 3];
        }
        m_midTuneStruct.tmk += TKIV->m_Bda_temp.temperature;
        ++i;
        emit setGeneralProgressBarCount(i);
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
    vlyout->addWidget(WDFunc::NewLBL(this, "Ввод значений сигналов c Энергомонитора"));

    vlyout->addWidget(WDFunc::NewLBLAndLE(this, "Uэт", "ValuetuneU", true));
    vlyout->addWidget(WDFunc::NewLBLAndLE(this, "Iэт", "ValuetuneI", true));
    vlyout->addWidget(WDFunc::NewLBLAndLE(this, "Yэт", "ValuetuneY", true));
    vlyout->addWidget(WDFunc::NewLBLAndLE(this, "fэт:", "ValuetuneF", true));
    QPushButton *pb = new QPushButton("Настроить");
    connect(pb, &QPushButton::clicked, this, &TuneKIVTemp60::saveIntermediateResults);
    vlyout->addWidget(pb);
    dlg->setLayout(vlyout);
    dlg->exec();
    return Error::Msg::NoError;
}

Error::Msg TuneKIVTemp60::calcTuneCoefsAndWrite()
{
    MidTuneStruct tunepositive;

    loadWorkConfig();
    loadIntermediateResults(tunepositive);
    // m_midTuneStruct ~ tunenegative
}

void TuneKIVTemp60::loadIntermediateResults(MidTuneStruct &tunestr)
{
    struct tunedescrstruct
    {
        QString parametername;
        float *parameter;
    };

    QVector<tunedescrstruct> tuneDescrVector;
    for (int i = 0; i < 6; ++i)
        tuneDescrVector.append({ "u_p" + QString::number(i), &tunestr.u[i] });
    for (int i = 0; i < 3; ++i)
        tuneDescrVector.append({ "y_p" + QString::number(i), &tunestr.y[i] });
    tuneDescrVector.append({ "tmk_p", &tunestr.tmk });
    QString cpuserialnum = Board::GetInstance().UID();
    QSettings storedcalibrations(StdFunc::GetSystemHomeDir() + "calibr.ini", QSettings::IniFormat);
    foreach (tunedescrstruct item, tuneDescrVector)
        *item.parameter
            = StdFunc::toFloat(storedcalibrations.value(cpuserialnum + "/" + item.parametername).toString());
}

void TuneKIVTemp60::saveIntermediateResults()
{
    struct tunedescrstruct
    {
        QString parametername;
        float *parameter;
    };

    QVector<tunedescrstruct> tuneDescrVector;
    for (int i = 0; i < 6; ++i)
        tuneDescrVector.append({ "u_p" + QString::number(i), &m_midTuneStruct.u[i] });
    for (int i = 0; i < 3; ++i)
        tuneDescrVector.append({ "y_p" + QString::number(i), &m_midTuneStruct.y[i] });
    tuneDescrVector.append({ "tmk_p", &m_midTuneStruct.tmk });
    tuneDescrVector.append({ "uet_p", &m_midTuneStruct.uet });
    tuneDescrVector.append({ "iet_p", &m_midTuneStruct.iet });
    tuneDescrVector.append({ "yet_p", &m_midTuneStruct.yet });
    QString cpuserialnum = Board::GetInstance().UID();
    QSettings storedcalibrations(StdFunc::GetSystemHomeDir() + "calibr.ini", QSettings::IniFormat);
    foreach (tunedescrstruct item, tuneDescrVector)
        storedcalibrations.setValue(cpuserialnum + "/" + item.parametername, *item.parameter);
    loadWorkConfig();
}
