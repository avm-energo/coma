#include "tunekivr.h"

#include "../../gen/colors.h"
#include "../../gen/stdfunc.h"
#include "../../widgets/waitwidget.h"
#include "../../widgets/wd_func.h"

#include <QMessageBox>
#include <QVBoxLayout>

TuneKIVR::TuneKIVR(int tuneStep, ConfigKIV *ckiv, TuneKIV *kiv, QWidget *parent) : AbstractTuneDialog(tuneStep, parent)
{
    CKIV = ckiv;
    TKIV = kiv;
    //    m_tuneStep = 1;
    //    SetBac(TKIV->m_Bac, 1, sizeof(TKIV->m_Bac));
    SetBac(TKIV->m_Bac);
    //    SetupUI();
    //    m_isEnergoMonitorDialogCreated = false;
    SetupUI();
}

void TuneKIVR::setMessages()
{
    m_messages.append("1. Ввод пароля...");
    m_messages.append("2. Отображение предупреждения...");
    m_messages.append("3. Запрос настроечных параметров...");
    m_messages.append("4. Проверка настроечных параметров...");
    m_messages.append("5. Настройка канала измерения температуры (КИТ): установка 80 Ом...");
    m_messages.append("6. Настройка КИТ: обработка...");
    m_messages.append("7. Настройка канала измерения температуры (КИТ): установка 120 Ом...");
    m_messages.append("8. Настройка КИТ: обработка и запись коэффициентов...");
}

void TuneKIVR::setTuneFunctions()
{
    int count = 0;
    m_tuneFunctions[m_messages.at(count++)]
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::CheckPassword);
    Error::Msg (AbstractTuneDialog::*func)()
        = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVR::showPreWarning);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&AbstractTuneDialog::readTuneCoefs);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVR::checkTuneCoefs);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVR::setR80);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVR::processR80);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVR::setR120);
    m_tuneFunctions[m_messages.at(count++)] = func;
    func = reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(&TuneKIVR::processR120);
    m_tuneFunctions[m_messages.at(count++)] = func;
}

// void TuneKIVMain::FillBac(int bacnum)
//{
//    TKIV->updateBacWidget();
//    Q_UNUSED(bacnum)
//}

// void TuneKIVMain::FillBackBac(int bacnum)
//{
//    TKIV->updateFromBacWidget();
//    Q_UNUSED(bacnum)
//}

QWidget *TuneKIVR::MainUI()
{
    QWidget *w = new QWidget;
    QVBoxLayout *lyout = new QVBoxLayout;
    QTabWidget *tw = new QTabWidget;
    tw->setObjectName("tunetw");
    QString ConfTWss = "QTabBar::tab:selected {background-color: " + QString(Colors::Tab) + ";}";
    tw->tabBar()->setStyleSheet(ConfTWss);
    tw->addTab(TKIV->m_Bac->widget(), "Настроечные параметры");
    tw->addTab(TKIV->BdaWidget(), "Текущие данные");
    lyout->addWidget(tw);
    w->setLayout(lyout);
    return w;
}

Error::Msg TuneKIVR::showPreWarning()
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

Error::Msg TuneKIVR::checkTuneCoefs()
{
    QVector<float *> tcoefs = { &TKIV->m_Bac->data()->KmU[0], &TKIV->m_Bac->data()->KmI1[0],
        &TKIV->m_Bac->data()->KmI2[0], &TKIV->m_Bac->data()->KmI4[0], &TKIV->m_Bac->data()->KmI8[0],
        &TKIV->m_Bac->data()->KmI16[0], &TKIV->m_Bac->data()->KmI32[0] };
    for (int i = 0; i < 3; ++i)
    {
        foreach (float *coef, tcoefs)
            if (!StdFunc::floatIsWithinLimits(this, *(coef + i), 1.0, 0.05))
                return Error::Msg::GeneralError;
    }
    if (!StdFunc::floatIsWithinLimits(this, TKIV->m_Bac->data()->K_freq, 1.0, 0.05))
        return Error::Msg::GeneralError;
    for (int i = 0; i < 6; ++i)
    {
        if (!StdFunc::floatIsWithinLimits(this, TKIV->m_Bac->data()->DPsi[i], 0.0, 1.0))
            return Error::Msg::GeneralError;
    }
    return Error::Msg::NoError;
}

Error::Msg TuneKIVR::setR80()
{
    setR(80);
    return Error::Msg::NoError;
}

Error::Msg TuneKIVR::processR80()
{
    m_pt100 = processR();
    if (StdFunc::isCancelled())
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}

Error::Msg TuneKIVR::setR120()
{
    setR(120);
    return Error::Msg::NoError;
}

Error::Msg TuneKIVR::processR120()
{
    double pt100_120 = processR();
    if (StdFunc::isCancelled())
        return Error::Msg::GeneralError;
    if (StdFunc::floatIsWithinLimits(this, pt100_120, m_pt100, 40, false))
    {
        WARNMSG("Ошибка в полученных данных, значения сопротивлений равны");
        StdFunc::cancel();
        return Error::Msg::GeneralError;
    }
    TKIV->m_Bac->data()->Art = (pt100_120 - m_pt100) / 40;
    TKIV->m_Bac->data()->Brt = pt100_120 * 2 - m_pt100 * 3;
    TKIV->m_Bac->updateWidget();
    //    TKIV->updateBacWidget();
    saveAllTuneCoefs();
    writeTuneCoefs();
    return Error::Msg::NoError;
}

void TuneKIVR::setR(int r)
{
    if (QMessageBox::question(this, "Подтверждение", "Установите сопротивление " + QString::number(r, 'f', 1) + " Ом")
        == QMessageBox::No)
        CancelTune();
}

double TuneKIVR::processR()
{
    //    startWait();
    emit setProgressSize(StdFunc::tuneRequestCount());
    int i = 0;
    double pt100 = 0.0;
    while ((!StdFunc::isCancelled()) && (i < StdFunc::tuneRequestCount()))
    {
        BaseInterface::iface()->reqBlockSync(1, DataTypes::DataBlockTypes::BdaBlock, &TKIV->m_Bda, sizeof(TKIV->m_Bda));
        TKIV->updateBdaWidget();
        pt100 += TKIV->m_Bda.Pt100;
        ++i;
        emit setProgressCount(i);
        StdFunc::Wait(500);
    }
    if (StdFunc::isCancelled())
        return 0;
    //    stopWait();
    pt100 /= i;
    return pt100;
}

void TuneKIVR::showTWTab(int num)
{
    QTabWidget *tw = this->findChild<QTabWidget *>("tunetw");
    if (tw != nullptr)
        tw->setCurrentIndex(num);
}

// void TuneKIVMain::saveIntermediateResults()
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

void TuneKIVR::setDefCoefs()
{
    TKIV->m_Bac->setDefBlockAndUpdate();
}
