#pragma once

#include "basejournal.h"

class SysJournal : public BaseJournal
{
private:
    virtual void fillTable(const QByteArray &ba) override;

public:
    explicit SysJournal(QObject *parent = nullptr);
};
