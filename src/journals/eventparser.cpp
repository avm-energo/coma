#include "eventparser.h"

#include <gen/timefunc.h>

namespace journals
{

EventParser::EventParser(QObject *parent) : QObject(parent)
{
}

QString EventParser::eventTypeToString(const EventType type)
{
    QString eventTypeStr;
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
    default:
        eventTypeStr = "Некорректное событие";
        break;
    }
    return eventTypeStr;
}

void EventParser::sortBinaryFile()
{
    auto unaryPredicate = [this](const auto &value) { return isBadRecord(value); };
    auto &firstRecord = m_eventFile.first();
    auto &lastRecord = m_eventFile.last();
    auto isFirstBad = isBadRecord(firstRecord);
    auto isLastBad = isBadRecord(lastRecord);

    // Two segments at the start and the end of file
    if (isFirstBad && isLastBad)
    {
        auto range { m_eventFile.findRange(unaryPredicate) };
        if (range)
            m_eventFile.remove(range);
        range = m_eventFile.findRange(unaryPredicate);
        if (range)
            m_eventFile.remove(range);
    }
    // One segment at the start or the end of file
    else if (isFirstBad || isLastBad)
    {
        auto range { m_eventFile.findRange(unaryPredicate) };
        if (range)
            m_eventFile.remove(range);
    }
    // One segment somewhere in the file
    else
    {
        auto range { m_eventFile.findRange(unaryPredicate) };

#ifdef DEBUG_JOURNALS
        std::ptrdiff_t indexStart = range.begin - m_eventFile.begin(), //
            indexEnd = range.end - m_eventFile.begin(),                //
            rangeSize = indexEnd - indexStart,                         //
            fileSize = m_eventFile.end() - m_eventFile.begin();        //
        qDebug() << "indexStart, indexEnd, rangeSize, fileSize: "      //
                 << indexStart << ' ' << indexEnd << ' ' << rangeSize  //
                 << ' ' << fileSize;                                   //
#endif

        if (range)
        {
#ifdef DEBUG_JOURNALS
            rangeSize = range.end - range.begin;
#else
            auto rangeSize = range.end - range.begin;
#endif
            auto result = m_eventFile.move(range.end, m_eventFile.end(), m_eventFile.begin());
            if (!result)
                return;
            range = { m_eventFile.end() - rangeSize, m_eventFile.end() };

#ifdef DEBUG_JOURNALS
            indexStart = range.begin - m_eventFile.begin(); //
            indexEnd = range.end - m_eventFile.begin();     //
            rangeSize = indexEnd - indexStart;
            fileSize = m_eventFile.end() - m_eventFile.begin();           //
            qDebug() << "indexStart, indexEnd, rangeSize, fileSize: "     //
                     << indexStart << ' ' << indexEnd << ' ' << rangeSize //
                     << ' ' << fileSize;                                  //
#endif

            if (range)
                m_eventFile.remove(range);

#ifdef DEBUG_JOURNALS
            lastRecord = m_eventFile.last();
            range = m_eventFile.findRange(unaryPredicate);
            indexStart = range.begin - m_eventFile.begin(); //
            indexEnd = range.end - m_eventFile.begin();     //
            rangeSize = indexEnd - indexStart;
            fileSize = m_eventFile.end() - m_eventFile.begin();           //
            qDebug() << "indexStart, indexEnd, rangeSize, fileSize: "     //
                     << indexStart << ' ' << indexEnd << ' ' << rangeSize //
                     << ' ' << fileSize;                                  //
#endif
        }
    }
}

bool EventParser::isBadRecord(const EventRecord &record)
{
    auto isBadTime = record.time == std::numeric_limits<quint64>::max();
    auto isBadType = record.type == EventType::Incorrect;
    auto isBadReserve = record.reserv == std::numeric_limits<quint32>::max();
    return (isBadTime && isBadType && isBadReserve);
}

void EventParser::update(const QByteArray &ba)
{
    m_records.clear();
    m_eventFile.update(ba);
    sortBinaryFile();
    m_records.reserve(m_eventFile.size());
}

JournalData EventParser::parse(const Descriptions &desc, const QTimeZone timeZone)
{
    JournalData retVal;
    retVal.reserve(m_eventFile.size());
    int count = 0;
    for (auto iter = m_eventFile.begin(); iter != m_eventFile.end(); ++iter)
    {
        const auto &record { *iter };
        if (isBadRecord(record))
            continue;
        ++count;

        quint32 index = (((record.num[2] << 8) + record.num[1]) << 8) + record.num[0];
        index = (index & 0x00FFFFFF);
        auto eventDesc = desc.value(index, "Некорректный номер события: " + QString::number(index));
        auto eventType = eventTypeToString(record.type);
        m_records.push_back(EventView { count, record.time, eventDesc, eventType, QString::number(record.reserv, 16) });
    }
    std::transform(
        m_records.cbegin(), m_records.cend(), std::back_inserter(retVal), [timeZone](const EventView &event) {
            return QVector<QVariant> { event.counter, TimeFunc::UnixTime64ToInvStringFractional(event.time, timeZone),
                event.desc, event.direction, event.hexField };
        });
    return retVal;
}

}
