#include "tune/kiv/tunekivtemp60.h"

#include <QEventLoop>
#include <QMessageBox>
#include <QVBoxLayout>
#include <gen/colors.h>
#include <gen/stdfunc.h>
#include <interfaces/conn/sync_connection.h>
#include <tune/tunesequencefile.h>
#include <tune/tunesteps.h>
#include <widgets/epopup.h>
#include <widgets/waitwidget.h>
#include <widgets/wd_func.h>

using namespace Interface;

TuneKIVTemp60::TuneKIVTemp60(int tuneStep, Device::CurrentDevice *device, QWidget *parent)
    : AbstractTuneDialog(tuneStep, device, parent)
    , m_bac(new BacA284(this))
    , m_bdain(new BdaIn(this))
    , m_bd0(new Bd0(this))
{
    m_bac->setup(m_device->getUID(), m_sync);
    m_bdain->setup(m_device->getUID(), m_sync);
    m_bd0->setup(m_device->getUID(), m_sync);

    TuneSequenceFile::clearTuneDescrVector();
    for (int i = 0; i < 6; ++i)
        TuneSequenceFile::addItemToTuneDescrVector("u_p" + QString::number(i), m_midTuneStruct.u[i]);
    for (int i = 0; i < 3; ++i)
        TuneSequenceFile::addItemToTuneDescrVector("y_p" + QString::number(i), m_midTuneStruct.y[i]);
    TuneSequenceFile::addItemToTuneDescrVector("tmk_p", m_midTuneStruct.tmk);
    TuneSequenceFile::addItemToTuneDescrVector("uet_p", m_midTuneStruct.uet);
    TuneSequenceFile::addItemToTuneDescrVector("iet_p", m_midTuneStruct.iet);
    TuneSequenceFile::addItemToTuneDescrVector("yet_p", m_midTuneStruct.yet);

    // CKIV = ckiv;
    setBac(m_bac);
    addWidgetToTabWidget(m_bac->widget(), "Настроечные параметры");
    addWidgetToTabWidget(m_bdain->widget(), "Текущие данные");
    addWidgetToTabWidget(m_bd0->widget(), "Общие данные");
    setupUI();
}

void TuneKIVTemp60::setTuneFunctions()
{
    addTuneFunc("1. Ввод пароля...", &AbstractTuneDialog::CheckPassword);
    addTuneFunc("2. Сохранение конфигурации...", &AbstractTuneDialog::saveWorkConfig);
    addTuneFunc("3. Задание временной конфигурации и настроечных параметров...", &TuneKIVTemp60::setNewConfAndTune);
    addTuneFunc("4. Выдача диалога о температуре в камере...", &TuneKIVTemp60::showTempDialog);
    addTuneFunc("5. Ожидание установления температурного режима...", &TuneKIVTemp60::waitForTempToRise);
    addTuneFunc("6. Диалог об установлении входных сигналов...", &TuneKIVTemp60::showSignalsDialog);
    addTuneFunc("7. Измерения...", &TuneKIVTemp60::analogMeasurement);
    if (m_tuneStep == KIVTS_60TUNING)
        addTuneFunc("8. Ввод данных энергомонитора и сохранение промежуточных данных...",
            &TuneKIVTemp60::inputEnergomonitorValues);
    else
    {
        addTuneFunc("8. Ввод данных энергомонитора...", &TuneKIVTemp60::inputEnergomonitorValues);
        addTuneFunc("9. Запись коэффициентов в модуль...", &TuneKIVTemp60::writeTuneCoefs);
    }
}

Error::Msg TuneKIVTemp60::setNewConfAndTune()
{
    config.setRecord("C_Pasp_ID", S2::FLOAT_3t { 2250, 2250, 2250 });
    config.setRecord("Unom1", float(220));
    auto s2file = config.toByteArray();
    auto status = m_sync->writeFileSync(S2::FilesEnum::Config, s2file);
    if (status != Error::Msg::NoError)
        return Error::Msg::GeneralError;

    for (int i = 0; i < 6; ++i)
    {
        m_bac->data()->TKUa[i] = 0;
        m_bac->data()->TKUb[i] = 0;
    }
    m_bac->data()->TKPsi_a[0] = m_bac->data()->TKPsi_a[1] = m_bac->data()->TKPsi_a[2] = 0;
    m_bac->data()->TKPsi_b[0] = m_bac->data()->TKPsi_b[1] = m_bac->data()->TKPsi_b[2] = 0;
    if (writeTuneCoefs(false) != Error::Msg::NoError)
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}

Error::Msg TuneKIVTemp60::showTempDialog()
{
    //    QDialog *dlg = new QDialog;
    //    QVBoxLayout *lyout = new QVBoxLayout;

    //    QWidget *w = new QWidget;
    QString tempstr = (m_tuneStep == KIVTS_60TUNING) ? "+60" : "-20";
    //    lyout->addWidget(
    //        WDFunc::NewLBL2(this, "Поместите модуль в термокамеру, установите температуру " + tempstr + " ± 2 °С"));
    //    lyout->addWidget(WDFunc::NewPB(this, "", "Готово", [&dlg] { dlg->close(); }));
    //    lyout->addWidget(WDFunc::NewPB(this, "cancelpb", "Отмена", [&dlg] { dlg->close(); }));
    //    w->setLayout(lyout);
    if (!EMessageBox::next(this, "Поместите модуль в термокамеру, установите температуру " + tempstr + " ± 2 °С"))
        CancelTune();
    //    dlg->setLayout(lyout);
    //    WDFunc::PBConnect(dlg, "cancelpb", static_cast<AbstractTuneDialog *>(this),
    //    &AbstractTuneDialog::CancelTune); dlg->exec();
    return Error::Msg::NoError;
}

Error::Msg TuneKIVTemp60::waitForTempToRise()
{
    WaitWidget *ww = new WaitWidget;
    ww->setObjectName("ww");
#ifndef NO_LIMITS
    const int time = 1800;
#else
    const int time = 15;
#endif
    WaitWidget::ww_struct wws = { true, false, WaitWidget::WW_TIME,
        time }; // isallowedtostop = true, isIncrement = false, format: mm:ss, 30 minutes
    ww->Init(wws);
    ww->SetMessage("Пожалуйста, подождите");
    StdFunc::SetCancelDisabled(); // to prevent cancellation of the main algorythm while breaking waiting
    QEventLoop loop;
    connect(ww, &WaitWidget::finished, &loop, &QEventLoop::quit);
    ww->Start();
    loop.exec();
    StdFunc::SetCancelEnabled();
    return Error::Msg::NoError;
}

Error::Msg TuneKIVTemp60::showSignalsDialog()
{
    QWidget *w = new QWidget(this);
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addWidget(WDFunc::NewIcon(this, ":/tunes/tunekiv1.png"));
    lyout->addWidget(WDFunc::NewLBL2(this, "1. Соберите схему подключения по одной из вышеприведённых картинок;"));
    lyout->addWidget(WDFunc::NewLBL2(this,
        "2. Включите питание Энергомонитор 3.1КМ и настройте его на режим измерения тока"
        "и напряжения в однофазной сети переменного тока, установите предел измерения"
        "по напряжению 60 В, по току - 2,5 А;"));
    lyout->addWidget(WDFunc::newHLine(w));

    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    vlyout->addWidget(WDFunc::NewLBL2(w, "РЕТОМ-51"));
    vlyout->addWidget(WDFunc::newHLine(w));
    vlyout->addWidget(WDFunc::NewLBL2(w,
        "Задайте трёхфазный режим токов и напряжений (Uabc, Iabc)\n"
        "Угол между токами и напряжениями: 89.9 град.,\n"
        "Значения напряжений: 57.75 В, токов: 140 мА"));
    hlyout->addLayout(vlyout);
    hlyout->addWidget(WDFunc::newVLine(w));
    vlyout = new QVBoxLayout;
    vlyout->addWidget(WDFunc::NewLBL2(w, "ИМИТАТОР"));
    vlyout->addWidget(WDFunc::newHLine(w));
    vlyout->addWidget(WDFunc::NewLBL2(w, "Задайте tg 2 %, значения напряжений: 57.75 В, токов: 140 мА"));
    hlyout->addLayout(vlyout);
    lyout->addLayout(hlyout);
    w->setLayout(lyout);

    if (!EMessageBox::next(this, w))
        CancelTune();
    return Error::Msg::NoError;
}

Error::Msg TuneKIVTemp60::analogMeasurement()
{
    emit setProgressSize(StdFunc::TuneRequestCount());
    for (int i = 0; i < 6; ++i)
    {
        m_midTuneStruct.u[i] = 0.0;
        if (i < 3)
            m_midTuneStruct.y[i] = 0.0;
    }
    m_midTuneStruct.tmk = 0.0;
    int reqCount = 0;
    while ((!StdFunc::IsCancelled()) && (reqCount < StdFunc::TuneRequestCount()))
    {
        m_bd0->readAndUpdate();
        m_bdain->readAndUpdate();
        for (int j = 0; j < 6; ++j)
        {
            m_midTuneStruct.u[j] += m_bdain->data()->IUefNat_filt[j];
            if (j < 3)
                m_midTuneStruct.y[j] += m_bdain->data()->phi_next_f[j + 3];
        }
        m_midTuneStruct.tmk += m_bd0->data()->Tmk;
        ++reqCount;
        emit setProgressCount(reqCount);
        StdFunc::Wait(500);
    }
    for (int i = 0; i < 6; ++i)
    {
        m_midTuneStruct.u[i] /= StdFunc::TuneRequestCount();
        if (i < 3)
            m_midTuneStruct.y[i] /= StdFunc::TuneRequestCount();
    }
    m_midTuneStruct.tmk /= StdFunc::TuneRequestCount();
    if (StdFunc::IsCancelled())
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}

Error::Msg TuneKIVTemp60::inputEnergomonitorValues()
{
    EEditablePopup *popup = new EEditablePopup("Ввод значений сигналов c Энергомонитора");
    //    QDialog *dlg = new QDialog(this);
    //    dlg->setObjectName("energomonitordlg");

    //    QVBoxLayout *vlyout = new QVBoxLayout;
    //    vlyout->addWidget(WDFunc::NewLBL2(this, "Ввод значений сигналов c Энергомонитора"));

    popup->addFloatParameter("Uэт, В", &m_midTuneStruct.uet);
    popup->addFloatParameter("Iэт, мА", &m_midTuneStruct.iet);
    popup->addFloatParameter("Yэт, град", &m_midTuneStruct.yet);
    //    vlyout->addWidget(WDFunc::NewLBLAndLE(this, "Uэт, В", "ValuetuneU", true));
    //    vlyout->addWidget(WDFunc::NewLBLAndLE(this, "Iэт, мА", "ValuetuneI", true));
    //    vlyout->addWidget(WDFunc::NewLBLAndLE(this, "Yэт, град", "ValuetuneY", true));
    //    QPushButton *pb = new QPushButton("Продолжить");
    if (EMessageBox::editableNext(popup))
    {
        if (m_tuneStep == KIVTS_60TUNING)
            saveIntermediateResults();
        else
            calcTuneCoefs();
    }
    else
        CancelTune();
    //    if (m_tuneStep == KIVTS_60TUNING)
    //        connect(pb, &QPushButton::clicked, [&dlg, this]() {
    //            saveIntermediateResults();
    //            dlg->close();
    //        });
    //    else
    //        connect(pb, &QPushButton::clicked, [&dlg, this]() {
    //            calcTuneCoefs();
    //            dlg->close();
    //        });
    //    vlyout->addWidget(pb);
    //    dlg->setLayout(vlyout);
    //    dlg->exec();
    return Error::Msg::NoError;
}

Error::Msg TuneKIVTemp60::calcTuneCoefs()
{
    //    m_midTuneStruct.uet = StdFunc::toFloat(WDFunc::LEData(this, "ValuetuneU"));
    //    m_midTuneStruct.iet = StdFunc::toFloat(WDFunc::LEData(this, "ValuetuneI"));
    //    m_midTuneStruct.yet = StdFunc::toFloat(WDFunc::LEData(this, "ValuetuneY"));
    MidTuneStruct tunenegative = m_midTuneStruct;

    loadWorkConfig();
    loadIntermediateResults();
    MidTuneStruct tunepositive = m_midTuneStruct;

    float dTp, dTm;

    dTp = tunepositive.tmk - m_bac->data()->Tmk0;
    dTm = tunenegative.tmk - m_bac->data()->Tmk0;

    for (int i = 0; i < 6; i++)
    {
        float dUp = (tunepositive.u[i] / tunepositive.uet) - 1;
        float dUm = (tunenegative.u[i] / tunenegative.uet) - 1;
        m_bac->data()->TKUa[i] = ((dUm * dTp * dTp) - (dUp * dTm * dTm)) / (dTp * dTm * (dTp - dTm));
        m_bac->data()->TKUb[i] = ((dUp * dTm) - (dUm * dTp)) / (dTp * dTm * (dTp - dTm));
    }

    for (int i = 0; i < 3; i++)
    {
        float dYp = (tunepositive.y[i] - tunepositive.yet);
        float dYm = (tunenegative.y[i] - tunenegative.yet);
        m_bac->data()->TKPsi_a[i] = ((dYm * dTp * dTp) - (dYp * dTm * dTm)) / (dTp * dTm * (dTp - dTm));
        m_bac->data()->TKPsi_b[i] = ((dYp * dTm) - (dYm * dTp)) / (dTp * dTm * (dTp - dTm));
    }
    m_bac->updateWidget();
    return Error::Msg::NoError;
}

void TuneKIVTemp60::loadIntermediateResults()
{
    TuneSequenceFile::loadItemsFromFile();
    //    for (TuneDescrStruct item : m_tuneDescrVector())
    //        *item.parameter = StdFunc::toFloat(TuneSequenceFile::value(item.parametername, 0.0).toString());
}

void TuneKIVTemp60::saveIntermediateResults()
{
    //    m_midTuneStruct.uet = StdFunc::toFloat(WDFunc::LEData(this, "ValuetuneU"));
    //    m_midTuneStruct.iet = StdFunc::toFloat(WDFunc::LEData(this, "ValuetuneI"));
    //    m_midTuneStruct.yet = StdFunc::toFloat(WDFunc::LEData(this, "ValuetuneY"));
    //    QString cpuserialnum = Board::GetInstance().UID();
    //    QSettings storedcalibrations(StdFunc::GetSystemHomeDir() + "calibr.ini", QSettings::IniFormat);
    //    for (const TuneDescrStruct &item : m_tuneDescrVector())
    //        TuneSequenceFile::setValue(item.parametername, *item.parameter);
    TuneSequenceFile::saveItemsToFile();
    loadWorkConfig();
}
