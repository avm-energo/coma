#include "eventparser.h"

#include <gen/timefunc.h>

namespace journals
{

EventParser::EventParser(QObject *parent) : QObject(parent)
{
}

QString EventParser::eventTypeToString(const EventType type)
{
    auto eventTypeStr = QString("");
    switch (type)
    {
    case EventType::Out:
        eventTypeStr = "Ушло";
        break;
    case EventType::In:
        eventTypeStr = "Пришло";
        break;
    case EventType::ChangeState:
        eventTypeStr = "Изменение состояния";
        break;
    }
    return eventTypeStr;
}

void EventParser::update(const QByteArray &ba)
{
    records.clear();
    iter = reinterpret_cast<const EventRecord *>(ba.constData());
    size = ba.size() / sizeof(EventRecord);
    records.reserve(size);
}

const Data EventParser::parse(const Descriptions &desc, const QTimeZone timeZone)
{
    Data retVal;
    retVal.reserve(size);

    int i, count;
    for (i = 0, count = 0; i < size; i++)
    {
        auto record = iter[i];
        if (record.time == ULLONG_MAX)
            continue;
        count++;

        int index = (((record.num[2] << 8) + record.num[1]) << 8) + record.num[0];
        index = (index & 0x00FFFFFF);
        auto eventDesc = desc.value(index, "Некорректный номер события: " + QString::number(index));
        auto eventType = eventTypeToString(record.type);
        records.push_back(EventView { count, record.time, eventDesc, eventType, QString::number(record.reserv, 16) });
    }
    std::sort(records.begin(), records.end(), //
        [](const EventView &lhs, const EventView &rhs) { return lhs.time > rhs.time; });
    std::transform(records.cbegin(), records.cend(), std::back_inserter(retVal), [timeZone](const EventView &event) {
        return QVector<QVariant> { event.counter, TimeFunc::UnixTime64ToInvStringFractional(event.time, timeZone),
            event.desc, event.direction, event.hexField };
    });
    return retVal;
}

}
