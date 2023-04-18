#pragma once

#include "basejournal.h"
#include "measparser.h"

namespace journals
{

class MeasJournal : public BaseJournal
{
private:
    MeasSettings settings;
    UniquePointer<MeasParser> parser;

    QStringList getHeaders();
    virtual void fillTable(const QByteArray &ba) override;

public:
    explicit MeasJournal(const MeasSettings &settings, QObject *parent = nullptr);
};

}
