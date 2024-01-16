#include "tune82adc.h"

#include "../../widgets/epopup.h"
#include "../../widgets/waitwidget.h"
#include "../../widgets/wd_func.h"
#include "../tunesteps.h"

#include <QMessageBox>
#include <QVBoxLayout>
#include <gen/colors.h>
#include <gen/stdfunc.h>

Tune82ADC::Tune82ADC(S2::Configuration &config, Modules::MezzanineBoard type, int tuneStep, QWidget *parent)
    : AbstractTuneDialog(config, tuneStep, parent)
{
    m_typeM = type;
    m_bac = new Bac82(this);
    m_bd1 = new Bd182(type, this);
    m_bda = new Bda82(this);
    m_bd0 = new Bd0(this);
    setBac(m_bac);
    m_BacWidgetIndex = addWidgetToTabWidget(m_bac->widget(), "Настроечные параметры");
    m_BdaWidgetIndex = addWidgetToTabWidget(m_bda->widget(), "Текущие данные");
    m_Bd0WidgetIndex = addWidgetToTabWidget(m_bd0->widget(), "Общие данные");
    //    m_isEnergoMonitorDialogCreated = false;
    m_curTuneStep = 0;
    setupUI();
}

void Tune82ADC::setTuneFunctions()
{
    addTuneFunc("Ввод пароля...", &AbstractTuneDialog::CheckPassword);
    addTuneFunc("Сохранение текущей конфигурации...", &AbstractTuneDialog::saveWorkConfig);
    addTuneFunc("Установка настроечных коэффициентов по умолчанию...", &Tune82ADC::setDefBac);
    addTuneFunc("Получение текущих аналоговых данных и их проверка...", &Tune82ADC::getAnalogData);
    addTuneFunc("Сохранение значений фильтра...", &Tune82ADC::saveUeff);
    addTuneFunc("Расчёт коррекции смещений сигналов по фазе...", &Tune82ADC::calcPhaseCorrection);
    addTuneFunc("Расчёт взаимного влияния каналов...", &Tune82ADC::calcInterChannelCorrelation);
    addTuneFunc("Расчёт коррекции смещения по токам и напряжениям...", &Tune82ADC::calcIUcoef1);
    if (m_typeM != Modules::MezzanineBoard::MTM_83) // not 6U0I
        addTuneFunc("Расчёт коррекции смещения по токам 5 А...", &Tune82ADC::calcIcoef5);
    addTuneFunc(
        "Запись настроечных коэффициентов и восстановление конфигурации...", &AbstractTuneDialog::writeTuneCoefs);
    addTuneFunc("Проверка регулировки...", &Tune82ADC::checkTune);
}

Error::Msg Tune82ADC::setDefBac()
{
    m_bac->setDefBlock();
    return Error::Msg::NoError;
}

Error::Msg Tune82ADC::getAnalogData()
{
    waitNSeconds(1);
    m_bda->readAndUpdate();
    waitNSeconds(1);
    const auto inom = config["I2nom"].value<S2::FLOAT_6t>();
    return m_bda->checkValues(m_typeM, inom);
}

Error::Msg Tune82ADC::saveUeff()
{
    // сохраняем значения по п. 7.3.2 для выполнения п. 7.3.6
    for (int i = 0; i < 6; i++)
        IUefNat_filt_old[i] = m_bd1->data()->IUefNat_filt[i];
    return Error::Msg::NoError;
}

Error::Msg Tune82ADC::calcPhaseCorrection()
{
    getBd1();
    for (int i = 1; i < 6; ++i)
    {
        m_bacNewBlock.data()->DPsi[i] = m_bac->data()->DPsi[i] - m_bd1->data()->phi_next_f[i];
    }
    if (m_typeM == Modules::MezzanineBoard::MTM_82)
    {
        for (int i = 3; i < 6; ++i)
            m_bacNewBlock.data()->DPsi[i] = m_bac->data()->DPsi[i] + mipdata.loadAnglePhase[i - 3];
    }
    m_bacNewBlock.data()->K_freq = m_bac->data()->K_freq / mipdata.freq[0];
    return Error::Msg::NoError;
}

Error::Msg Tune82ADC::calcInterChannelCorrelation()
{
    showPreWarning();
    m_bd1->readBlockFromModule();
    float fTmp = 0;
    for (int i : { 0, 3 })
    {
        fTmp += (m_bd1->data()->IUefNat_filt[i] / IUefNat_filt_old[i]);
    }
    fTmp /= 2;
    m_bacNewBlock.data()->Kinter = (fTmp * (1 + 6 * m_bac->data()->Kinter) - 1) / 6;
    return Error::NoError;
}

Error::Msg Tune82ADC::calcIUcoef1()
{
    saveWorkConfig();
    // set nominal currents in config to 1.0 A
    setCurrentsTo(1.0);
    if (!EMessageBox::next(this, "Задайте напряжения равными 60,0 В и токи, равными 1,0 А"))
    {
        CancelTune();
        return Error::GeneralError;
    }
    getBd1();
    for (int i = 0; i < 3; ++i)
    {
        switch (m_typeM)
        {
        case Modules::MezzanineBoard::MTM_83: // 0I6U
            m_bacNewBlock.data()->KmU[i] = m_bac->data()->KmU[i] * mipdata.uPhase[i] / m_bd1->data()->IUefNat_filt[i];
            m_bacNewBlock.data()->KmU[i + 3]
                = m_bac->data()->KmU[i + 3] * mipdata.uPhase[i] / m_bd1->data()->IUefNat_filt[i + 3];
            break;
        case Modules::MezzanineBoard::MTM_82: // 3I3U
            m_bacNewBlock.data()->KmU[i] = m_bac->data()->KmU[i] * mipdata.uPhase[i] / m_bd1->data()->IUefNat_filt[i];
            m_bacNewBlock.data()->KmI_1[i + 3]
                = m_bac->data()->KmI_1[i + 3] * mipdata.iPhase[i] / m_bd1->data()->IUefNat_filt[i + 3];
            break;
        case Modules::MezzanineBoard::MTM_81: // 6I0U
            m_bacNewBlock.data()->KmI_1[i]
                = m_bac->data()->KmI_1[0] * mipdata.iPhase[i] / m_bd1->data()->IUefNat_filt[i];
            m_bacNewBlock.data()->KmI_1[i + 3]
                = m_bac->data()->KmI_1[i + 3] * mipdata.iPhase[i] / m_bd1->data()->IUefNat_filt[i + 3];
            break;
        default:
            break;
        }
    }
    return Error::NoError;
}

Error::Msg Tune82ADC::calcIcoef5()
{
    // set nominal currents in config to 5.0 A
    setCurrentsTo(5.0);
    if (!EMessageBox::next(this, "Задайте токи, равными 5,0 А"))
    {
        CancelTune();
        return Error::GeneralError;
    }
    getBd1();
    for (int i = 0; i < 3; ++i)
    {
        switch (m_typeM)
        {
        case Modules::MezzanineBoard::MTM_82: // 3I3U
            m_bacNewBlock.data()->KmI_5[i + 3]
                = m_bac->data()->KmI_5[i + 3] * mipdata.iPhase[i] / m_bd1->data()->IUefNat_filt[i + 3];
            break;
        case Modules::MezzanineBoard::MTM_81: // 6I0U
            m_bacNewBlock.data()->KmI_5[i]
                = m_bac->data()->KmI_5[0] * mipdata.iPhase[i] / m_bd1->data()->IUefNat_filt[i];
            m_bacNewBlock.data()->KmI_5[i + 3]
                = m_bac->data()->KmI_5[i + 3] * mipdata.iPhase[i] / m_bd1->data()->IUefNat_filt[i + 3];
            break;
        default:
            break;
        }
    }
    return Error::NoError;
}

Error::Msg Tune82ADC::showPreWarning()
{
    QVBoxLayout *lyout = new QVBoxLayout;

    QWidget *w = new QWidget(this);
    lyout->addWidget(WDFunc::NewLBL2(this, "", "", new QPixmap(":/tunes/tune82.png")));
    lyout->addWidget(WDFunc::NewLBL2(this, "1. Соберите схему подключения по одной из вышеприведённых картинок;"));
    lyout->addWidget(WDFunc::NewLBL2(this,
        "2. Задайте на РЕТОМ трехфазный режим токов и напряжений с углами сдвига"
        "в фазах А токов и напряжений 0 градусов, в фазах В - 240, в фазах С - 120 градусов,"
        "НЕ МЕНЯЯ ЗНАЧЕНИЙ НАПРЯЖЕНИЙ И ТОКОВ!"));
    w->setLayout(lyout);

    if (!EMessageBox::next(this, w))
    {
        CancelTune();
        return Error::GeneralError;
    }
    return Error::NoError;
}

Error::Msg Tune82ADC::checkTune()
{
    getBd1();
    EMessageBox::information(this,
        "После закрытия данного сообщения для завершения настройки нажмите Enter\nДля отказа от настройки нажмите Esc");
    m_finished = false;
    while ((!StdFunc::IsCancelled()) && !m_finished)
    {
        m_bda->readAndUpdate();
        StdFunc::Wait(500);
    }
    if (StdFunc::IsCancelled())
        return Error::Msg::GeneralError;
    return Error::Msg::NoError;
}

void Tune82ADC::setCurrentsTo(float i)
{
    saveWorkConfig();
    // set nominal currents in config to i A
    S2::FLOAT_6t i2NomConfig { i, i, i, i, i, i };
    config.setRecord("I2nom", i2NomConfig);
}

void Tune82ADC::getBd1()
{
    Mip *mip = new Mip(false, m_typeM, this);
    auto inom = config["I2nom"].value<S2::FLOAT_6t>();
    mipdata = mip->takeOneMeasurement(inom.at(3));
    m_bd1->readBlockFromModule();
}
