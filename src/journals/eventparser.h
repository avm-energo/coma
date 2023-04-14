#pragma once

#include <QByteArray>
#include <QObject>

class EventParser : public QObject
{
    Q_OBJECT
public:
    EventParser(QObject *parent = nullptr);
};
