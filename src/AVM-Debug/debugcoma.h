#pragma once

#include "../widgets/coma.h"

class DebugComa : public Coma
{
    Q_OBJECT

public:
    DebugComa(QWidget *parent = nullptr);
    ~DebugComa();
    void PrepareDialogs() override;
    void getAbout() override;
    void setupConnection() override;
    void checkDialog() override;
};
