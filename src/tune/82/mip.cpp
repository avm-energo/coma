#include "mip.h"

#include "../gen/datamanager/typesproxy.h"
#include "../gen/settings.h"
#include "../gen/stdfunc.h"
#include "../interfaces/settingstypes.h"
#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"

#include <QSettings>

Mip::Mip(bool withGUI, MType moduleType, QWidget *parent) : QObject()
{
    m_parent = parent;
    m_moduleType = moduleType;
    if (withGUI)
    {
        setupWidget();
        m_widget->engine()->addFloat({ 0, 46 });
        connect(m_widget->engine(), &ModuleDataUpdater::itsTimeToUpdateFloatSignal, this, &Mip::updateData);
        //        connect(m_widget->proxyFS.get(), &DataTypesProxy::DataStorable, this, &Mip::updateData,
        //        Qt::QueuedConnection);
    }
}

void Mip::updateData(const DataTypes::FloatStruct &fl)
{
    if (fl.sigAdr < 46)
    {
        float *mipdata = reinterpret_cast<float *>(&m_mipData);
        *(mipdata + fl.sigAdr) = fl.sigVal;
    }
}

Mip::MipDataStruct Mip::getData()
{
    return m_mipData;
}

void Mip::setupWidget()
{
    m_widget = new UWidget;
    QGridLayout *lyout = new QGridLayout;
    for (int i = 0; i < 3; ++i)
    {
        lyout->addWidget(WDFunc::NewLBLAndLBL(m_widget, QString::number(i + 1),
                             "Частота ф. " + QString::number(i + 10, 36).toUpper(), true),
            0, i);
        lyout->addWidget(WDFunc::NewLBLAndLBL(m_widget, QString::number(i + 4),
                             "Фазное напряжение ф. " + QString::number(i + 10, 36).toUpper(), true),
            1, i);
        lyout->addWidget(WDFunc::NewLBLAndLBL(m_widget, QString::number(i + 19),
                             "Фазное напряжение ф. " + QString::number(i + 10, 36).toUpper(), true),
            2, i);
        lyout->addWidget(WDFunc::NewLBLAndLBL(m_widget, QString::number(i + 7),
                             "Фазный ток ф. " + QString::number(i + 10, 36).toUpper(), true),
            3, i);
    }
    lyout->addWidget(WDFunc::NewLBLAndLBL(m_widget, "10", "Ток N", true), 4, 0);
    for (int i = 0; i < 3; ++i)
    {
        lyout->addWidget(WDFunc::NewLBLAndLBL(m_widget, QString::number(i + 11),
                             "Угол ф. " + QString::number(i + 10, 36).toUpper(), true),
            5, i);
        lyout->addWidget(WDFunc::NewLBLAndLBL(m_widget, QString::number(i + 22),
                             "Активная мощность ф. " + QString::number(i + 10, 36).toUpper(), true),
            6, i);
        lyout->addWidget(WDFunc::NewLBLAndLBL(m_widget, QString::number(i + 26),
                             "Реактивная мощность ф. " + QString::number(i + 10, 36).toUpper(), true),
            7, i);
        lyout->addWidget(WDFunc::NewLBLAndLBL(m_widget, QString::number(i + 30),
                             "Полная мощность ф. " + QString::number(i + 10, 36).toUpper(), true),
            8, i);
    }
    lyout->addWidget(WDFunc::NewLBLAndLBL(m_widget, "17", "Температура", true), 9, 0);
    lyout->addWidget(WDFunc::NewPB(m_widget, "", "Далее",
                         [&] {
                             emit finished();
                             m_widget->close();
                         }),
        10, 0);
    m_widget->setLayout(lyout);
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

    assert((m_moduleType == MType::MTM_81) || (m_moduleType == MType::MTM_82) || (m_moduleType == MType::MTM_83));
    switch (m_moduleType)
    {
    case MType::MTM_81:
        u = 60.0;
        uthr = 0.05;
        iNom = 0;
        ithr = MAXFLOAT;
        break;
    case MType::MTM_82:
        u = 60.0;
        uthr = 0.05;
        ithr = 0.05;
        break;
    case MType::MTM_83:
        u = 0;
        uthr = MAXFLOAT;
        ithr = 0.05;
        break;
    default:
        return Error::GeneralError;
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
            EMessageBox::warning(m_parent,
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

void Mip::setModuleType(MType type)
{
    m_moduleType = type;
}

void Mip::setNominalCurrent(double inom)
{
    iNom = inom;
}

UWidget *Mip::widget()
{
    return m_widget;
}
