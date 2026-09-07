#include "tune/84/tune84check.h"

#include <datablocks/84/bda.h>
#include <libavm-gen/files.h>
#include <libavm-gen/stdfunc.h>
#include <libavm-widgets/emessagebox.h>
#include <libavm-widgets/graphfunc.h>
#include <libavm-widgets/lblfunc.h>
#include <libavm-widgets/waitwidget.h>
#include <libavm-widgets/wdfunc.h>

#include <QCoreApplication>
#include <QDialog>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

Tune84Check::Tune84Check(Device::CurrentDevice *device, QWidget *parent) : AbstractTuneDialog(device, parent)
{
    setupUI();
}

void Tune84Check::setTuneFunctions()
{
    addTuneFunc("Сохранение текущей конфигурации...", &AbstractTuneDialog::saveWorkConfig);
    addTuneFunc("Отображение схемы подключения...", &Tune84Check::showScheme);
    addTuneFunc("Проверка...", &Tune84Check::check);
}

Error::Msg Tune84Check::showScheme()
{
    QWidget *w = new QWidget(this);
    QVBoxLayout *lyout = new QVBoxLayout;

    QLabel *schema = GraphFunc::newIcon(this, ":/tunes/tunekiv1.png");
    schema->setScaledContents(true);
    schema->setMaximumSize(QSize(402, 582));
    lyout->addWidget(schema);

    lyout->addWidget(LBLFunc::New(this, "1. Соберите схему подключения по одной из вышеприведённых картинок;"));
    lyout->addWidget(LBLFunc::New(this,
        "2. Включите питание Энергомонитор 3.1КМ и настройте его на режим измерения тока"
        "и напряжения в однофазной сети переменного тока, установите предел измерения"
        "по напряжению 57,75 В, по току - 2,5 А;"));

    lyout->addWidget(LBLFunc::New(this,
        "3. Включите питание источника сигналов. Задайте напряжение U1 равным 57,75 В с углом 0 град.,\n"
        "ток I1 равным 2,9 А с углом 89,9 град., включите выходы РЕТОМ-51;"));

    lyout->addWidget(LBLFunc::New(this,
        "4. По показаниям Энергомонитора убедитесь, что входное напряжение от источника"
        "составляет 57,75 ± 0,25 В, ток – 0,500 ± 25 мА, частота – 51,0 ± 0,05 Гц;"));

    lyout->addWidget(LBLFunc::New(
        this, "5. Убедитесь, что частота мигания светодиода «Работа»  на лицевой панели увеличилась до 1 Гц;"));

    lyout->addWidget(LBLFunc::New(this, "6. Установите на магазине сопротивлений сопротивление 100,0 Ом."));

    w->setLayout(lyout);
    if (!EMessageBox::next(this, w))
        CancelTune();
    return Error::Msg::NoError;
}

Error::Msg Tune84Check::check()
{
    // Паспортная ёмкость и Unom1 нужны здесь как эталонные значения, иначе пересчитанный
    // из измеренного тока результат уйдёт за допуск и проверка выдаст "Ошибочное значение
    // коэффициента по току" - как это уже сделано для аналогичного модуля в TuneKIVCheck::check().
    std::vector<std::pair<QString, S2::valueType>> recordList {
        { "C_Pasp_ID", S2::FLOAT_3t { 9000, 9000, 9000 } }, //
        { "Unom1", float(220) }                             //
    };

    if (sendChangedConfig(recordList) != Error::Msg::NoError)
    {
        EMessageBox::error(this, "Ошибка при записи конфигурации");
        return Error::Msg::GeneralError;
    }

    WaitWidget *ww = new WaitWidget("", this);
    ww->setMessage("Пожалуйста, подождите");
    ww->start();
    QElapsedTimer *tmr = new QElapsedTimer;
    tmr->start();
    while (tmr->elapsed() < TIMEFORBDATOSETINMS)
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    ww->stop();

    Bda84 *bda = new Bda84(this);
    bda->setup(m_device->getUID(), m_sync);
    bda->readAndUpdate();
    // Возвращаем исходную (до эталонных значений выше) конфигурацию на устройство в любом случае,
    // независимо от результата проверки.
    auto s2file = m_config.toByteArray();

    Error::Msg result = Error::Msg::NoError;
#ifndef NO_LIMITS
    for (int i = 0; i < 3; ++i)
        if (!WDFunc::floatIsWithinLimits("напряжения", bda->data()->Ueff_ADC[i], 2069375.0, 144375.0))
            result = Error::Msg::DataError;
    for (int i = 3; i < 6; ++i)
        if (!WDFunc::floatIsWithinLimits("тока", bda->data()->Ueff_ADC[i], 1220000.0, 60000.0))
            result = Error::Msg::DataError;
    if (!WDFunc::floatIsWithinLimits("частоты", bda->data()->Frequency, 51.0, 0.05))
        result = Error::Msg::DataError;
#endif
    m_sync->writeFileSync(S2::FilesEnum::Config, s2file);
    return result;
}

void Tune84Check::showEvent(QShowEvent *e)
{
    e->accept();
}
