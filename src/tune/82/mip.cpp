#include "mip.h"

#include "../gen/datamanager.h"
#include "../gen/settings.h"
#include "../gen/stdfunc.h"
#include "../interfaces/settingstypes.h"
#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"

#include <QSettings>

// Mip::Mip(bool withGUI, AvtukVariants moduleType, QWidget *parent) : UDialog(withGUI, parent)
Mip::Mip(bool withGUI, AvtukVariants moduleType, QWidget *parent) : UWidget(parent)
{
    Q_UNUSED(withGUI);
    m_moduleType = moduleType;
    //    if (withGUI)
    setupUI();
    setFloatBdQuery({ { 0, 46 } });
    connect(&DataManager::GetInstance(), &DataManager::floatReceived, this, &UWidget::updateFloatData,
        Qt::QueuedConnection);
}

void Mip::updateFloatData(const DataTypes::FloatStruct &fl)
{
    if (fl.sigAdr < 46)
    {
        float *mipdata = reinterpret_cast<float *>(&m_mipData);
        *(mipdata + fl.sigAdr) = fl.sigVal;
    }
    UWidget::updateFloatData(fl);
}

Mip::MipDataStruct Mip::getData()
{
    return m_mipData;
}

void Mip::setupUI()
{
    QGridLayout *lyout = new QGridLayout;
    for (int i = 0; i < 3; ++i)
    {
        lyout->addWidget(WDFunc::NewLBLAndLBL(
                             this, QString::number(i + 1), "Частота ф. " + QString::number(i + 10, 36).toUpper(), true),
            0, i);
        lyout->addWidget(WDFunc::NewLBLAndLBL(this, QString::number(i + 4),
                             "Фазное напряжение ф. " + QString::number(i + 10, 36).toUpper(), true),
            1, i);
        lyout->addWidget(WDFunc::NewLBLAndLBL(this, QString::number(i + 19),
                             "Фазное напряжение ф. " + QString::number(i + 10, 36).toUpper(), true),
            2, i);
        lyout->addWidget(WDFunc::NewLBLAndLBL(this, QString::number(i + 7),
                             "Фазный ток ф. " + QString::number(i + 10, 36).toUpper(), true),
            3, i);
    }
    lyout->addWidget(WDFunc::NewLBLAndLBL(this, "10", "Ток N", true), 4, 0);
    for (int i = 0; i < 3; ++i)
    {
        lyout->addWidget(WDFunc::NewLBLAndLBL(
                             this, QString::number(i + 11), "Угол ф. " + QString::number(i + 10, 36).toUpper(), true),
            5, i);
        lyout->addWidget(WDFunc::NewLBLAndLBL(this, QString::number(i + 22),
                             "Активная мощность ф. " + QString::number(i + 10, 36).toUpper(), true),
            6, i);
        lyout->addWidget(WDFunc::NewLBLAndLBL(this, QString::number(i + 26),
                             "Реактивная мощность ф. " + QString::number(i + 10, 36).toUpper(), true),
            7, i);
        lyout->addWidget(WDFunc::NewLBLAndLBL(this, QString::number(i + 30),
                             "Полная мощность ф. " + QString::number(i + 10, 36).toUpper(), true),
            8, i);
    }
    lyout->addWidget(WDFunc::NewLBLAndLBL(this, "17", "Температура", true), 9, 0);
    lyout->addWidget(WDFunc::NewPB(this, "", "Далее",
                         [this] {
                             emit finished();
                             this->close();
                         }),
        10, 0);
    setLayout(lyout);
}

void Mip::start()
{
    using namespace settings;
    auto sets = std::make_unique<QSettings>();
    m_device = UniquePointer<IEC104>(new IEC104);
    IEC104Settings settings;
    settings.ip = sets->value(regMap[MIPIP].name, regMap[MIPIP].defValue).toString();
    settings.baseadr = sets->value(regMap[MIPAddress].name, regMap[MIPAddress].defValue).toUInt();
    ConnectStruct st { "mip", settings };
    m_device->start(st);
}

void Mip::stop()
{
    m_device->stop();
}

Error::Msg Mip::check()
{
    double ithr, u, uthr;

    assert((m_moduleType == M81) || (m_moduleType == M82) || (m_moduleType == M83));
    switch (m_moduleType)
    {
    case M81:
        u = 60.0;
        uthr = 0.05;
        iNom = 0;
        ithr = MAXFLOAT;
        break;
    case M82:
        u = 60.0;
        uthr = 0.05;
        ithr = 0.05;
        break;
    case M83:
        u = 0;
        uthr = MAXFLOAT;
        ithr = 0.05;
        break;
    }

    double ValuesToCheck[10] = { 0.0, 50.0, 50.0, 50.0, u, u, u, iNom, iNom, iNom };
    double ThresholdsToCheck[10] = { 0.0, 0.05, 0.05, 0.05, uthr, uthr, uthr, ithr, ithr, ithr };
    double *VTC, *TTC;
    VTC = ValuesToCheck;
    TTC = ThresholdsToCheck;
    for (int i = 0; i < 10; i++)
    {
        float *mipdata = reinterpret_cast<float *>(&m_mipData);
        if (!StdFunc::FloatIsWithinLimits(*(mipdata + i), *VTC, *TTC))
        {
            EMessageBox::warning(this,
                "Несовпадение МИП по параметру " + QString::number(i)
                    + ". Измерено: " + QString::number(*mipdata, 'f', 4)
                    + ", должно быть: " + QString::number(*VTC, 'f', 4) + " +/- " + QString::number(*TTC, 'f', 4));
            return Error::Msg::GeneralError;
        }
        ++VTC;
        ++TTC;
    }
    return Error::Msg::NoError;
}

void Mip::setModuleType(AvtukVariants type)
{
    m_moduleType = type;
}

void Mip::setNominalCurrent(double inom)
{
    iNom = inom;
}
