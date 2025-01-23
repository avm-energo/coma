#include "tune/21/tune21one.h"

#include <gen/colors.h>
#include <gen/stdfunc.h>
#include <tune/tunetypes.h>
#include <widgets/epopup.h>
#include <widgets/waitwidget.h>
#include <widgets/wdfunc.h>

#include <QMessageBox>
#include <QVBoxLayout>

Tune21One::Tune21One(Device::BoardTypes boardType, u8 chNum, Device::CurrentDevice *device, QWidget *parent)
    : AbstractTuneDialog(device, parent)
    , m_channelNumber(chNum)
{
    u8 blockNum = (boardType == Device::BoardTypes::BASEBOARD) ? 1 : 2;
    m_bac = new Bac21(blockNum, this);
    m_bda = new Bda21(blockNum, this);
    m_bac->setup(m_device->getUID(), m_sync);
    m_bda->setup(m_device->getUID(), m_sync);

    setBac(m_bac);
    addWidgetToTabWidget(m_bda->widget(), "Текущие данные");
    setupUI();
}

void Tune21One::setTuneFunctions()
{
    addTuneFunc("Сохранение текущей конфигурации...", &AbstractTuneDialog::saveWorkConfig);
    addTuneFunc("Регулировка...", &Tune21One::tune);
    addTuneFunc("Запись настроечных коэффициентов и восстановление конфигурации...", &Tune21One::sendBac);
    addTuneFunc("Проверка регулировки...", &Tune21One::checkTuneCoefs);
}

Error::Msg Tune21One::checkTuneCoefs()
{
#ifndef NO_LIMITS
    if (!WDFunc::floatIsWithinLimits(
            "коэффициента тока по наклону", m_bac->data()->bac1[m_channelNumber].fkiin, 1.0, 0.1))
        return Error::Msg::Cancelled;
    if (!WDFunc::floatIsWithinLimits(
            "коэффициента напряжения по наклону", m_bac->data()->bac1[m_channelNumber].fkuin, 1.0, 0.1))
        return Error::Msg::Cancelled;
    if (!WDFunc::floatIsWithinLimits(
            "коэффициента тока по смещению", m_bac->data()->bac1[m_channelNumber].fbin_I, 0.0, 1000.0))
        return Error::Msg::Cancelled;
    if (!WDFunc::floatIsWithinLimits(
            "коэффициента напряжения по смещению", m_bac->data()->bac2[m_channelNumber].fbin_U, 0.0, 1000.0))
        return Error::Msg::Cancelled;
#endif
    return Error::Msg::NoError;
}

Error::Msg Tune21One::tune()
{
    float i0, i20, u0, u5;
    if (m_channelNumber > 7)
    {
        qDebug() << "Некорректный номер шага: " << m_channelNumber;
        return Error::Msg::StepError;
    }
    if (EMessageBox::next(this,
            "Включите режим измерения напряжений и\n"
            "на калибраторе задайте напряжение 0 В на\nвходе "
                + QString::number(m_channelNumber) + " модуля и нажмите Далее"))
    {
        m_bda->readAndUpdate();
        u0 = m_bda->data()->sin[m_channelNumber];
        if (EMessageBox::next(this,
                "На калибраторе задайте напряжение 5 В на\nвходе " + QString::number(m_channelNumber)
                    + " модуля и нажмите Далее"))
        {
            m_bda->readAndUpdate();
            u5 = m_bda->data()->sin[m_channelNumber];
            if (EMessageBox::next(this,
                    "Включите режим измерения токов и\n"
                    "на калибраторе задайте ток 0 мА на\nвходе "
                        + QString::number(m_channelNumber) + " модуля и нажмите Далее"))
            {
                m_bda->readAndUpdate();
                i0 = m_bda->data()->sin[m_channelNumber];
                if (EMessageBox::next(this,
                        "На калибраторе задайте ток 20 мА на\nвходе " + QString::number(m_channelNumber)
                            + " модуля и нажмите Далее"))
                {
                    m_bda->readAndUpdate();
                    i20 = m_bda->data()->sin[m_channelNumber];
                    if (!calcNewTuneCoefs(u0, u5, i0, i20))
                        return Error::Msg::TuneCoefError;
                    else
                        return Error::Msg::NoError;
                }
            }
        }
    }
    return Error::Msg::Cancelled;
}

bool Tune21One::calcNewTuneCoefs(float u0, float u5, float i0, float i20)
{
    m_bac->data()->bac1[m_channelNumber].fbin_I = 1.25f - i0;
    m_bac->data()->bac2[m_channelNumber].fbin_U = 1.25f - u0;
    if ((StdFunc::FloatIsWithinLimits(u0, u5, 0.1f) || (StdFunc::FloatIsWithinLimits(i0, i20, 0.1f))))
    {
        qDebug() << "Ошибка в настроечных коэффициентах, деление на ноль";
        return false;
    }
    m_bac->data()->bac1[m_channelNumber].fkuin = 1 / (u0 - u5);
    m_bac->data()->bac1[m_channelNumber].fkiin = 1 / (i0 - i20);
    return true;
}

Error::Msg Tune21One::sendBac()
{
    m_bac->updateWidget();
    if (writeTuneCoefs() != Error::Msg::NoError)
        return Error::Msg::WriteError;
    if (loadWorkConfig() != Error::Msg::NoError)
        return Error::Msg::ReadError;
    return Error::Msg::NoError;
}

Error::Msg Tune21One::showScheme()
{
    QVBoxLayout *lyout = new QVBoxLayout;

    QWidget *w = new QWidget(this);
    lyout->addWidget(GraphFunc::NewIcon(this, ":/tunes/tune21.png"));
    lyout->addWidget(LBLFunc::NewLBL(this, "Соберите схему подключения по вышеприведённой картинке"));
    w->setLayout(lyout);

    if (!EMessageBox::next(this, w))
    {
        CancelTune();
        return Error::Cancelled;
    }
    return Error::NoError;
}
