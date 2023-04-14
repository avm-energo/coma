#pragma once

#include "basejournal.h"

namespace journals
{

class WorkJournal : public BaseJournal
{
public:
    explicit WorkJournal(QObject *parent = nullptr);
};

}
