#include "baseeventsjournalmodel.h"

#include <gen/timefunc.h>

BaseEventsJournalModel::BaseEventsJournalModel(QObject *parent) : BaseJournalModel(parent)
{
}

QVector<QVector<QVariant>> BaseEventsJournalModel::createCommon(
    const QByteArray &array, const int eventid, const QStringList &desc)
{
    const auto basize = array.size();
    const char *file = array.constData();

    const auto recordsize = sizeof(EventStruct);
    QVector<QVector<QVariant>> ValueLists;
    ValueLists.reserve(basize / recordsize);

    int counter, i;
    std::vector<CommonEvent> events;
    for (i = 0, counter = 0; i < basize; i += recordsize)
    {
        EventStruct event;
        memcpy(&event, file, recordsize);
        file += recordsize;
        i += recordsize;
        if (event.Time == ULLONG_MAX)
            continue;
        ++counter;

        // TODO: FIX THAT
        int N = 0;
        memcpy(&N, &event.EvNum, sizeof(event.EvNum));
        N = (N & 0x00FFFFFF) - eventid;
        QString eventDesc;
        if ((N <= desc.size()) && (N >= 0))
        {
            eventDesc = desc.at(N);
        }
        else
            eventDesc = "Некорректный номер события: " + QString::number(N);

        QString eventType;
        if (event.EvType)
            eventType = "Пришло";
        else
            eventType = "Ушло";

        CommonEvent commonEvent { counter, event.Time, eventDesc, eventType, QString::number(event.Reserv, 16) };
        events.push_back(commonEvent);
    }
    std::sort(events.begin(), events.end(),
        [](const CommonEvent &lhs, const CommonEvent &rhs) { return lhs.time > rhs.time; });
    auto timeZone = TimeFunc::userTimeZone();
    std::transform(
        events.cbegin(), events.cend(), std::back_inserter(ValueLists), [timeZone](const CommonEvent &event) {
            return QVector<QVariant> { event.counter, TimeFunc::UnixTime64ToInvStringFractional(event.time, timeZone),
                event.desc, event.direction, event.hexField };
        });
    return ValueLists;
}
