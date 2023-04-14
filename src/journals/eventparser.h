#pragma once

#include "../module/modulesettings.h"

#include <QTimeZone>

namespace journals
{

enum EventType : quint8
{
    Out = 0,
    In = 1,
    ChangeState = 2
};

struct EventRecord
{
    quint64 time;
    EventType type;
    quint8 num[3];
    quint32 reserv;
};

struct EventView
{
    int counter;
    quint64 time;
    QString desc;
    QString direction;
    QString hexField; // доп.поле
};

using Descriptions = ModuleTypes::WorkJourMap;
using Data = QVector<QVector<QVariant>>;

class EventParser : public QObject
{
    Q_OBJECT
private:
    const EventRecord *iter;
    std::size_t size;
    QVector<EventView> records;

public:
    EventParser(QObject *parent = nullptr);
    void update(const QByteArray &ba);
    const Data parse(const Descriptions &desc, const QTimeZone timeZone);
};

}
