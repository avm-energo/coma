#pragma once

#include "basejournal.h"
#include "eventparser.h"

namespace journals
{

class WorkJournal : public BaseJournal
{
private:
    QMap<quint32, QString> desriptions;
    UniquePointer<EventParser> parser;

    virtual void fillTable(const QByteArray &ba) override;

public:
    explicit WorkJournal(const QMap<quint32, QString> &desc, QObject *parent = nullptr);
};

}
