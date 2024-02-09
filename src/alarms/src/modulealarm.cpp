#include "alarms/modulealarm.h"

#include "../../widgets/wd_func.h"

#include <QScrollArea>
#include <QVBoxLayout>

const std::map<AlarmType, QColor> ModuleAlarm::s_colors = {
    { AlarmType::Critical, Qt::red },   //
    { AlarmType::Warning, Qt::yellow }, //
    { AlarmType::Info, Qt::green }      //
};

ModuleAlarm::ModuleAlarm(const AlarmType type, const AlarmValue &alarms, QWidget *parent)
    : BaseAlarm(parent), m_alarms(std::move(alarms))
{
    auto search = s_colors.find(type);
    if (search != s_colors.cend())
        m_alarmColor = search->second;
    else
        m_alarmColor = Qt::transparent;
    setupUI(m_alarms.values());
}

void ModuleAlarm::followToData(const SignalMap &sigMap)
{
    auto &addr = m_alarms.cbegin().key();
    auto search = std::find_if(sigMap.cbegin(), sigMap.cend(), //
        [&addr](const Device::SigMapValue &element) -> bool {  //
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

void ModuleAlarm::setupUI(const QStringList &events)
{
    auto mainLayout = new QVBoxLayout(this);
    auto widget = new QWidget(this);
    auto vLayout = new QVBoxLayout(widget);
    widget->setLayout(vLayout);

    // Создаём labels и circles
    m_labelStateStorage.reserve(events.size());
    auto index = 0;
    for (auto &&desc : events)
    {
        auto hLayout = new QHBoxLayout;
        auto pixmap = WDFunc::NewCircle(m_normalColor, circleRadius);
        auto label = WDFunc::NewLBL2(this, "", QString::number(index), &pixmap);
        hLayout->addWidget(label);
        hLayout->addWidget(WDFunc::NewLBL2(this, desc), 1);
        vLayout->addLayout(hLayout);
        m_labelStateStorage.append({ label, false });
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
        const auto pixmap = WDFunc::NewCircle(color, circleRadius);
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
        for (auto it = m_alarms.keyBegin(); it != m_alarms.keyEnd(); it++, index++)
        {
            if (sp.sigAdr == *it)
            {
                updatePixmap(sigval & 0x00000001, index);
                break;
            }
        }
    }
}
