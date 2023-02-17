#ifndef BASEEVENTSJOURNALMODEL_H
#define BASEEVENTSJOURNALMODEL_H

#include "../models/edynamictablemodel.h"

#include <QObject>

class BaseEventsJournalClass
{
    struct CommonEvent
    {
        int counter;
        quint64 time;
        QString desc;
        QString direction;
        QString hexField; // дополнительное поле
    };

    struct EventStruct
    {
        EventStruct() = default;
        quint64 Time;
        quint8 EvType;
        quint8 EvNum[3];
        quint32 Reserv;
    };

    Q_OBJECT
public:
    BaseEventsJournalClass();

    QVector<QVector<QVariant>> createCommon(const QByteArray &array, const int eventid, const QStringList &desc);
};

#endif // BASEEVENTSJOURNALMODEL_H
