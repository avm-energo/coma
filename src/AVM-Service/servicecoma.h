#pragma once

#include "../widgets/coma.h"

class ServiceComa : public Coma
{
    Q_OBJECT

public:
    ServiceComa(QWidget *parent = nullptr);
    ~ServiceComa();
    void PrepareDialogs() override;
    void getAbout() override;
    void setupConnection() override;
};
