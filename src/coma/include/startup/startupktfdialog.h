#pragma once

#include <startup/abstractstartupdialog.h>

class StartupKTFDialog : public AbstractStartupDialog
{
    Q_OBJECT

public:
    explicit StartupKTFDialog(Device::CurrentDevice *device, QWidget *parent = nullptr);
    virtual ~StartupKTFDialog();

private:
    struct WBd7
    {
        float InitAge;
    };

    struct Bd9
    {
        float Vst;
        float Age;
        float Resurs;
        float reserv;
    };

    Bd9 *Bd9Block;
    WBd7 *WBd7Block;

    // void GetCorBd() override;
    // void WriteCor() override;
    // void SetupCor() override;
    // void ResetCor() override;
    // void FillCor() override;
    // void FillBackCor() override;

    void SetupUI() override;
    void FillBd(QWidget *parent, QString Name, QString Value);

public slots:
    void SaveToFile() override;
    void ReadFromFile() override;
    bool checkStartupValues() override;

private slots:
};
