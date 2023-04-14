#pragma once

#include "basejournal.h"

namespace journals
{

class MeasJournal : public BaseJournal
{
public:
    explicit MeasJournal(QObject *parent = nullptr);
};

}
