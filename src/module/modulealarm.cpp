#include "modulealarm.h"

#include "../gen/datamanager/typesproxy.h"
#include "../module/configstorage.h"
#include "../widgets/wd_func.h"

#include <QScrollArea>
#include <QVBoxLayout>

ModuleAlarm::ModuleAlarm(const Modules::AlarmType &type, //
    const ModuleTypes::AlarmValue &alarms, QWidget *parent)
    : BaseAlarm(parent), mAlarms(std::move(alarms)), mProxy(new DataTypesProxy)
{
    static const QHash<Modules::AlarmType, QColor> colors = {
        { Modules::AlarmType::Critical, Qt::red },   //
        { Modules::AlarmType::Warning, Qt::yellow }, //
        { Modules::AlarmType::Info, Qt::green }      //
    };

    followToData();
    alarmColor = colors.value(type, Qt::transparent);
    mProxy->RegisterType<DataTypes::SinglePointWithTimeStruct>();
    connect(mProxy.get(), &DataTypesProxy::DataStorable, this, qOverload<const QVariant &>(&ModuleAlarm::update));
    setupUI(mAlarms.values());
    engine()->setUpdatesEnabled();
}

/// \brief Folowing the data: search a signal group whose range
/// includes the address of the first alarm from the list.
void ModuleAlarm::followToData()
{
    auto &sigMap = ConfigStorage::GetInstance().getModuleSettings().getSignals();
    auto &addr = mAlarms.cbegin().key();
    auto search = std::find_if(sigMap.cbegin(), sigMap.cend(), //
        [&addr](const ModuleTypes::Signal &signal) -> bool {   //
            auto acceptStart = signal.startAddr;
            auto acceptEnd = acceptStart + signal.count;
            return (addr >= acceptStart && addr < acceptEnd);
        });
    if (search != sigMap.cend())
    {
        auto &signal = search.value();
        engine()->addSp({ signal.startAddr, signal.count });
    }
}

/// \brief Setup UI: creating text labels and indicators (pixmaps) for alarms displaying.
void ModuleAlarm::setupUI(const QStringList &events)
{
    auto lyout = new QVBoxLayout(this);
    auto widget = new QWidget(this);
    auto vlayout = new QVBoxLayout(widget);
    widget->setLayout(vlayout);
    // Создаём labels и circles
    auto index = 0;
    for (auto &&desc : events)
    {
        auto hlyout = new QHBoxLayout;
        auto label = WDFunc::NewLBL2(this, "", QString::number(index));
        auto pixmap = WDFunc::NewCircle(normalColor, circleRadius);
        label->setPixmap(pixmap);
        hlyout->addWidget(label);
        hlyout->addWidget(WDFunc::NewLBL2(this, desc), 1);
        vlayout->addLayout(hlyout);
        index++;
    }
    // Создаём QScrollArea
    auto scrollArea = new QScrollArea(this);
    scrollArea->setWidget(widget);
    lyout->addWidget(scrollArea);
    // Создаём кнопку "Ок"
    auto pb = new QPushButton("Ok", this);
    QObject::connect(pb, &QPushButton::clicked, this, &ModuleAlarm::hide);
    lyout->addWidget(pb);
    setLayout(lyout);
}

/// \brief Update a indicator (pixmap) for alarms displaying.
void ModuleAlarm::updatePixmap(const bool &isset, const quint32 &position)
{
    auto color = isset ? alarmColor : normalColor;
    auto pixmap = WDFunc::NewCircle(color, circleRadius);
    auto status = WDFunc::SetLBLImage(this, QString::number(position), &pixmap);
    if (!status)
        qCritical() << Error::DescError;
    emit updateColor(color);
}

/// \brief The slot called when a SinglePoint data is received from the communication protocol.
void ModuleAlarm::update(const QVariant &msg)
{
    auto sp = msg.value<DataTypes::SinglePointWithTimeStruct>();
    const quint8 sigval = sp.sigVal;
    if (!(sigval & 0x80))
    {
        quint32 index = 0;
        bool foundFlag = false;
        for (auto it = mAlarms.keyBegin(); it != mAlarms.keyEnd(); it++, index++)
        {
            if (sp.sigAdr == *it)
            {
                foundFlag = true;
                break;
            }
        }
        if (foundFlag)
            updatePixmap(sigval & 0x00000001, index);
    }
}
