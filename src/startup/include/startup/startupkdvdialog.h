#pragma once

#include <startup/abstractstartupdialog.h>

class StartupKDVDialog : public AbstractStartupDialog
{
    Q_OBJECT
public:
    StartupKDVDialog(Device::CurrentDevice *device, QWidget *parent = nullptr);
    virtual ~StartupKDVDialog();

private:
    struct WBd7
    {
        float InitAge;
    };

    struct WBd8
    {
        float MHnInit;
        float MotHovInit;
    };
    struct Bd9
    {
        float Vst;
        float Age;
        float Resurs;
        float MotHnorm;
        float MotHover;
        float MotHTotal;
        float reserv;
    };

    Bd9 *Bd9Block;
    WBd7 *WBd7Block;
    WBd8 *WBd8Block;

    // void GetCorBd() override;
    // void FillBackCor() override;
    // void FillCor() override;
    // void FillBackWBd8();
    // void WriteCor() override;
    // void SetupCor() override;
    // void ResetCor() override;

    void SetupUI() override;
    void FillBd(QWidget *parent, QString Name, QString Value);

public slots:
    void SaveToFile() override;
    void ReadFromFile() override;
    bool checkStartupValues() override;
};
