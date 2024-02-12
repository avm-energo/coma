#include "tunekivr.h"

#include "../../widgets/epopup.h"
#include "../../widgets/waitwidget.h"
#include "../../widgets/wd_func.h"

#include <QMessageBox>
#include <QVBoxLayout>
#include <gen/colors.h>
#include <gen/stdfunc.h>

TuneKIVR::TuneKIVR(int tuneStep, Device::CurrentDevice *device, QWidget *parent)
    : AbstractTuneDialog(tuneStep, device, parent)
{
    m_bac = new Bac2A284(this);
    m_bda = new BdaA284(this);
    setBac(m_bac);
    m_BacWidgetIndex = addWidgetToTabWidget(m_bac->widget(), "Настроечные параметры");
    m_BdaWidgetIndex = addWidgetToTabWidget(m_bda->widget(), "Текущие данные");
    setupUI();
}

void TuneKIVR::setTuneFunctions()
{
    addTuneFunc("1. Ввод пароля...", &AbstractTuneDialog::CheckPassword);
    addTuneFunc("2. Отображение предупреждения...", &TuneKIVR::showPreWarning);
    addTuneFunc("3. Запрос настроечных параметров...", &AbstractTuneDialog::readTuneCoefs);
    addTuneFunc("4. Настройка канала измерения температуры (КИТ): установка 80 Ом...", &TuneKIVR::setR80);
    addTuneFunc("5. Настройка КИТ: обработка...", &TuneKIVR::processR80);
    addTuneFunc("6. Настройка канала измерения температуры (КИТ): установка 120 Ом...", &TuneKIVR::setR120);
    addTuneFunc("7. Настройка КИТ: обработка и запись коэффициентов...", &TuneKIVR::processR120);
}

Error::Msg TuneKIVR::showPreWarning()
{
    showTWTab(m_BacWidgetIndex);
    auto widget = new QWidget(this);
    auto layout = new QVBoxLayout;

    layout->addWidget(WDFunc::NewIcon(this, ":/tunes/tunekiv1.png"), 0);
    layout->addWidget(WDFunc::NewLBL2(this, "1. Соберите схему подключения по одной из вышеприведённых картинок;"));
    layout->addWidget(WDFunc::NewLBL2(this,
        "2. Включите питание Энергомонитор 3.1КМ и настройте его на режим измерения тока "
        "и напряжения в однофазной сети переменного тока, установите предел измерения "
        "по напряжению 60 В, по току - 2,5 А;"));
    layout->addWidget(WDFunc::NewLBL2(this,
        "3. Данный этап регулировки должен выполняться при температуре "
        "окружающего воздуха +20±7 °С. Если температура окружающего воздуха отличается от указанной, "
        "разместите модуль в термокамеру с диапазоном регулирования температуры "
        "от минус 20 до +60°С. Установите нормальное значение температуры "
        "в камере 20±5°С"));
    widget->setLayout(layout);
    if (!EMessageBox::next(this, widget))
    {
        CancelTune();
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
    if (StdFunc::IsCancelled())
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
    if (StdFunc::IsCancelled())
        return Error::Msg::GeneralError;
#ifndef NO_LIMITS
    if (WDFunc::floatIsWithinLimits("сопротивления", pt100_120, m_pt100, 40, false))
    {
        WARNMSG("Ошибка в полученных данных, значения сопротивлений равны");
        StdFunc::Cancel();
        return Error::Msg::GeneralError;
    }
#endif
    m_bac->data()->Art = (pt100_120 - m_pt100) / 40;
    m_bac->data()->Brt = pt100_120 * 2 - m_pt100 * 3;
    m_bac->updateWidget();
    showTWTab(m_BacWidgetIndex);
    saveAllTuneCoefs();
    return writeTuneCoefs();
}

void TuneKIVR::setR(int r)
{
    if (!EMessageBox::next(this, "Установите сопротивление " + QString::number(r, 'f', 1) + " Ом"))
        CancelTune();
}

double TuneKIVR::processR()
{
    showTWTab(m_BdaWidgetIndex);
    emit setProgressSize(StdFunc::TuneRequestCount());
    int i = 0;
    double pt100 = 0.0;
    while ((!StdFunc::IsCancelled()) && (i < StdFunc::TuneRequestCount()))
    {
        m_bda->readAndUpdate();
        pt100 += m_bda->data()->Pt100;
        ++i;
        emit setProgressCount(i);
        StdFunc::Wait(500);
    }
    if (StdFunc::IsCancelled())
        return 0;
    pt100 /= i;
    return pt100;
}
