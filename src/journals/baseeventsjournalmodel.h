#ifndef BASEEVENTSJOURNALMODEL_H
#define BASEEVENTSJOURNALMODEL_H

#include "../models/edynamictablemodel.h"
#include "basejournalmodel.h"

#include <QObject>

class BaseEventsJournalModel : public BaseJournalModel
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
    BaseEventsJournalModel(QObject *parent = nullptr);

    QVector<QVector<QVariant>> createCommon(const QByteArray &array, const int eventid, const QStringList &desc);
    void FillEventsTable(const QByteArray &ba);

private:
    EDynamicTableModel *m_model;
};

#endif // BASEEVENTSJOURNALMODEL_H
