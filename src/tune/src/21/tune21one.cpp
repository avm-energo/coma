#include "tune/21/tune21one.h"

#include <gen/colors.h>
#include <gen/stdfunc.h>
#include <tune/tunetypes.h>
#include <widgets/epopup.h>
#include <widgets/graphfunc.h>
#include <widgets/lblfunc.h>
#include <widgets/waitwidget.h>
#include <widgets/wdfunc.h>

#include <QMessageBox>
#include <QVBoxLayout>

Tune21One::Tune21One(Device::BoardTypes boardType, Device::CurrentDevice *device, QWidget *parent)
    : AbstractTuneDialog(device, parent)
{
    u8 blockNum = (boardType == Device::BoardTypes::BASEBOARD) ? 1 : 2;
    m_bac = new Bac21(blockNum, this);
    m_bda = new Bda21(blockNum, this);
    m_bac->setup(m_device->getUID(), m_sync);
    m_bda->setup(m_device->getUID(), m_sync);

    setBac(m_bac);
    m_bac->createWidget();
    addWidgetToTabWidget(m_bda->widget(), "Текущие данные");
    setupUI();
}

void Tune21One::setTuneFunctions()
{
    addTuneFunc("Сохранение текущей конфигурации...", &AbstractTuneDialog::saveWorkConfig);
    addTuneFunc("Регулировка 0 В...", &Tune21One::tune0);
    addTuneFunc("Регулировка 5 В...", &Tune21One::tune5);
    addTuneFunc("Регулировка 4 мА...", &Tune21One::tune4);
    addTuneFunc("Регулировка 20 мА...", &Tune21One::tune20);
    addTuneFunc("Расчёт коэффициентов...", &Tune21One::calcNewTuneCoefs);
    addTuneFunc("Запись настроечных коэффициентов и восстановление конфигурации...", &Tune21One::sendBac);
    addTuneFunc("Проверка регулировки...", &Tune21One::checkTuneCoefs);
}

Error::Msg Tune21One::checkTuneCoefs()
{
#ifndef NO_LIMITS
    for (int i = 0; i < 8; ++i)
    {
        if (!WDFunc::floatIsWithinLimits("коэффициента тока по наклону", m_bac->data()->bac1[i].fkiin, 1.0, 0.5))
            return Error::Msg::Cancelled;
        if (!WDFunc::floatIsWithinLimits("коэффициента напряжения по наклону", m_bac->data()->bac1[i].fkuin, 1.0, 0.5))
            return Error::Msg::Cancelled;
        if (!WDFunc::floatIsWithinLimits("коэффициента тока по смещению", m_bac->data()->bac1[i].fbin_I, 0.0, 1000.0))
            return Error::Msg::Cancelled;
        if (!WDFunc::floatIsWithinLimits(
                "коэффициента напряжения по смещению", m_bac->data()->bac2[i].fbin_U, 0.0, 1000.0))
            return Error::Msg::Cancelled;
    }
#endif
    return Error::Msg::NoError;
}

Error::Msg Tune21One::tune0()
{
    for (int i = 0; i < 8; ++i)
    {
        if (EMessageBox::next(this,
                "Включите режим измерения напряжений и\n"
                "на калибраторе задайте напряжение 0 В на\nвходе "
                    + QString::number(i + 1) + " модуля и нажмите Далее"))
        {
            m_bda->readAndUpdate();
            m_u0[i] = m_bda->data()->sin[i];
        }
        else
            return Error::Msg::Cancelled;
    }
    return Error::Msg::NoError;
}

Error::Msg Tune21One::tune4()
{
    for (int i = 0; i < 8; ++i)
    {
        if (EMessageBox::next(this,
                "Включите режим измерения токов и\n"
                "на калибраторе задайте ток 0 мА на\nвходе "
                    + QString::number(i + 1) + " модуля и нажмите Далее"))
        {
            m_bda->readAndUpdate();
            m_i0[i] = m_bda->data()->sin[i];
        }
        else
            return Error::Msg::Cancelled;
    }
    return Error::Msg::NoError;
}

Error::Msg Tune21One::tune5()
{
    for (int i = 0; i < 8; ++i)
    {
        if (EMessageBox::next(this,
                "На калибраторе задайте напряжение 5 В на\nвходе " + QString::number(i + 1)
                    + " модуля и нажмите Далее"))
        {
            m_bda->readAndUpdate();
            m_u5[i] = m_bda->data()->sin[i];
        }
        else
            return Error::Msg::Cancelled;
    }
    return Error::Msg::NoError;
}

Error::Msg Tune21One::tune20()
{
    for (int i = 0; i < 8; ++i)
    {
        if (EMessageBox::next(this,
                "На калибраторе задайте ток 20 мА на\nвходе " + QString::number(i + 1) + " модуля и нажмите Далее"))
        {
            m_bda->readAndUpdate();
            m_i20[i] = m_bda->data()->sin[i];
        }
        else
            return Error::Msg::Cancelled;
    }
    return Error::Msg::NoError;
}

Error::Msg Tune21One::calcNewTuneCoefs()
{
    for (int i = 0; i < 8; ++i)
    {
        m_bac->data()->bac1[i].fbin_I = 1.25f - m_i0[i];
        m_bac->data()->bac2[i].fbin_U = 1.25f - m_u0[i];
        if ((StdFunc::FloatIsWithinLimits(m_u0[i], m_u5[i], 0.1f)
                || (StdFunc::FloatIsWithinLimits(m_i0[i], m_i20[i], 0.1f))))
        {
            qDebug() << "Ошибка в настроечных коэффициентах, деление на ноль";
            return Error::Msg::TuneCoefError;
        }
        m_bac->data()->bac1[i].fkuin = 1 / (m_u0[i] - m_u5[i]);
        m_bac->data()->bac1[i].fkiin = 1 / (m_i0[i] - m_i20[i]);
    }
    return Error::Msg::NoError;
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
