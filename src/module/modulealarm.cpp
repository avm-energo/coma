#include "modulealarm.h"

#include "../module/configstorage.h"
#include "../widgets/wd_func.h"

#include <QScrollArea>
#include <QVBoxLayout>
#include <gen/datamanager/typesproxy.h>

const QHash<Modules::AlarmType, QColor> ModuleAlarm::colors = {
    { Modules::AlarmType::Critical, Qt::red },   //
    { Modules::AlarmType::Warning, Qt::yellow }, //
    { Modules::AlarmType::Info, Qt::green }      //
};

ModuleAlarm::ModuleAlarm(const Modules::AlarmType &type, //
    const ModuleTypes::AlarmValue &alarms, QWidget *parent)
    : BaseAlarm(parent), mAlarms(std::move(alarms)), mProxy(new DataTypesProxy)
{
    alarmColor = colors.value(type, Qt::transparent);
    followToData();
    mProxy->RegisterType<DataTypes::SinglePointWithTimeStruct>();
    connect(mProxy.get(), &DataTypesProxy::DataStorable, this, qOverload<const QVariant &>(&ModuleAlarm::update));
    setupUI(mAlarms.values());
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
    engine()->setUpdatesEnabled();
}

/// \brief Setup UI: creating text labels and indicators (pixmaps) for alarms displaying.
void ModuleAlarm::setupUI(const QStringList &events)
{
    auto mainLayout = new QVBoxLayout(this);
    auto widget = new QWidget(this);
    auto vLayout = new QVBoxLayout(widget);
    widget->setLayout(vLayout);

    // Создаём labels и circles
    labelStateStorage.reserve(events.size());
    auto index = 0;
    for (auto &&desc : events)
    {
        auto hLayout = new QHBoxLayout;
        auto pixmap = WDFunc::NewCircle(normalColor, circleRadius);
        auto label = WDFunc::NewLBL2(this, "", QString::number(index), &pixmap);
        hLayout->addWidget(label);
        hLayout->addWidget(WDFunc::NewLBL2(this, desc), 1);
        vLayout->addLayout(hLayout);
        labelStateStorage.append({ label, false });
        index++;
    }

    // Создаём QScrollArea
    auto scrollArea = new QScrollArea(this);
    scrollArea->setWidget(widget);
    mainLayout->addWidget(scrollArea);
    // Создаём кнопку "Ок"
    auto pb = new QPushButton("Ok", this);
    QObject::connect(pb, &QPushButton::clicked, this, &ModuleAlarm::hide);
    mainLayout->addWidget(pb);
    setLayout(mainLayout);
}

/// \brief Check if all pixmap labels inactive.
bool ModuleAlarm::isAllPixmapInactive() const
{
    auto retFlag = true;
    for (auto &&statePair : labelStateStorage)
    {
        if (statePair.second)
        {
            retFlag = false;
            break;
        }
    }
    return retFlag;
}

/// \brief Update a indicator (pixmap) for alarms displaying.
void ModuleAlarm::updatePixmap(const bool &isSet, const quint32 &position)
{
    if (labelStateStorage[position].second != isSet)
    {
        const auto color = isSet ? alarmColor : normalColor;
        auto label = labelStateStorage[position].first;
        const auto pixmap = WDFunc::NewCircle(color, circleRadius);
        label->setPixmap(pixmap);
        labelStateStorage[position].second = isSet;
        if (isSet or isAllPixmapInactive())
            emit updateColor(color);
    }
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
