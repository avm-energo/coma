#pragma once

#include "../widgets/coma.h"

class DebugComa : public Coma
{
    Q_OBJECT

public:
    DebugComa(QWidget *parent = nullptr);
    ~DebugComa();
    void PrepareDialogs() override;
};
