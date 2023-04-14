#pragma once

#include "basejournal.h"
#include "eventparser.h"

namespace journals
{

class SysJournal : public BaseJournal
{
private:
    static const QMap<quint32, QString> desriptions;
    UniquePointer<EventParser> parser;

    virtual void fillTable(const QByteArray &ba) override;

public:
    explicit SysJournal(QObject *parent = nullptr);
};

}
