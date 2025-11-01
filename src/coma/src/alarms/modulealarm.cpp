#include "alarms/modulealarm.h"

#include <avm-widgets/graphfunc.h>
#include <avm-widgets/lblfunc.h>

#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

ModuleAlarm::ModuleAlarm(const AlarmType type, const AlarmValue &alarms, //
    Device::CurrentDevice *device, QWidget *parent)
    : BaseAlarm(device, parent)
    , m_alarms(std::move(alarms))
{
    m_alarmColor = getColorByType(type);
    setupUI(m_alarms.values());
    setMinimumWidth(300);
}

void ModuleAlarm::followToData(const SignalMap &sigMap)
{
    auto addr = m_alarms.cbegin().value().sigAdr;
    auto search =
      std::find_if(sigMap.cbegin(), sigMap.cend(),                       //
                   [&addr](const Device::SigMapValue &element) -> bool { //
                     auto &signal = element.second;
                     auto acceptStart = signal.startAddr;
                     auto acceptEnd = acceptStart + signal.count;
                     return (addr >= acceptStart && addr < acceptEnd);
                   });
    if (search != sigMap.cend())
    {
        auto &signal = search->second;
        engine()->addSp({ signal.startAddr, signal.count });
    }
    engine()->setUpdatesEnabled();
}

void ModuleAlarm::setupUI(const QList<Device::XmlDataTypes::AlarmOne> &events)
{
    auto mainLayout = new QVBoxLayout(this);
    auto widget = new QWidget(this);
    auto vLayout = new QVBoxLayout(widget);

    // Создаём labels и circles
    m_labelStateStorage.reserve(events.size());
    auto index = 0;
    for (auto &&event : events)
    {
        auto hLayout = new QHBoxLayout;
        auto pixmap = GraphFunc::newCircle(m_normalColor, circleRadius);
        auto label = LBLFunc::New(this, "", QString::number(index), &pixmap);
        hLayout->addWidget(label, 1);
        hLayout->addWidget(LBLFunc::New(this, event.desc), 100);
        vLayout->addLayout(hLayout);
        m_labelStateStorage.append({ label, false });
        index++;
    }
    widget->setLayout(vLayout);

    // Создаём QScrollArea
    auto scrollArea = new QScrollArea(this);
    scrollArea->setWidget(widget);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);
    // Создаём кнопку "Ок"
    auto pb = new QPushButton("Ok", this);
    QObject::connect(pb, &QPushButton::clicked, this, &ModuleAlarm::hide);
    mainLayout->addWidget(pb);
    setLayout(mainLayout);
}

bool ModuleAlarm::isAllPixmapInactive() const
{
    for (auto &&statePair : m_labelStateStorage)
    {
        if (statePair.second)
            return false;
    }
    return true;
}

void ModuleAlarm::updatePixmap(const bool &isSet, const quint32 &position)
{
    if (m_labelStateStorage[position].second != isSet)
    {
        const auto color = isSet ? m_alarmColor : m_normalColor;
        auto label = m_labelStateStorage[position].first;
        const auto pixmap = GraphFunc::newCircle(color, circleRadius);
        label->setPixmap(pixmap);
        m_labelStateStorage[position].second = isSet;
        if (isSet or isAllPixmapInactive())
            emit updateColor(color);
    }
}

void ModuleAlarm::updateSPData(const DataTypes::SinglePointWithTimeStruct &sp)
{
    const quint8 sigval = sp.sigVal;
    if (!(sigval & 0x80))
    {
        quint32 index = 0;
        for (auto it = m_alarms.cbegin(), end = m_alarms.cend(); it != end; it++, index++)
        {
            auto second = it.value();
            if (sp.sigAdr == second.sigAdr)
            {
                updatePixmap(sigval & 0x00000001, index);
                // break;
            }
        }
    }
}
