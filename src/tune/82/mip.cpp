#include "mip.h"

#include "../gen/datamanager.h"
#include "../gen/settings.h"
#include "../interfaces/settingstypes.h"
#include "../widgets/wd_func.h"

#include <QSettings>

Mip::Mip(bool withGUI, QWidget *parent) : UWidget(withGUI, parent)
{
    if (withGUI)
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

Mip::MipDataStruct Mip::GetData()
{
    return m_mipData;
}

void Mip::setupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    for (int i = 0; i < 3; ++i)
    {
        lyout->addWidget(WDFunc::NewLBLAndLE(
            this, QString::number(i + 1), "Частота ф. " + QString::number(i + 10, 36).toUpper(), true));
        lyout->addWidget(WDFunc::NewLBLAndLE(
            this, QString::number(i + 4), "Фазное напряжение ф. " + QString::number(i + 10, 36).toUpper(), true));
        lyout->addWidget(WDFunc::NewLBLAndLE(
            this, QString::number(i + 19), "Фазное напряжение ф. " + QString::number(i + 10, 36).toUpper(), true));
        lyout->addWidget(WDFunc::NewLBLAndLE(
            this, QString::number(i + 7), "Фазный ток ф. " + QString::number(i + 10, 36).toUpper(), true));
    }
    lyout->addWidget(WDFunc::NewLBLAndLE(this, "10", "Ток N", true));
    for (int i = 0; i < 3; ++i)
    {
        lyout->addWidget(WDFunc::NewLBLAndLE(
            this, QString::number(i + 11), "Угол ф. " + QString::number(i + 10, 36).toUpper(), true));
        lyout->addWidget(WDFunc::NewLBLAndLE(
            this, QString::number(i + 22), "Активная мощность ф. " + QString::number(i + 10, 36).toUpper(), true));
        lyout->addWidget(WDFunc::NewLBLAndLE(
            this, QString::number(i + 26), "Реактивная мощность ф. " + QString::number(i + 10, 36).toUpper(), true));
        lyout->addWidget(WDFunc::NewLBLAndLE(
            this, QString::number(i + 30), "Полная мощность ф. " + QString::number(i + 10, 36).toUpper(), true));
    }
    lyout->addWidget(WDFunc::NewLBLAndLE(this, "17", "Температура", true));
    setLayout(lyout);
}

void Mip::Start()
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

void Mip::Stop()
{
    m_device->stop();
}
