#ifndef CORDIALOGKTF_H
#define CORDIALOGKTF_H

#include "abstractstartupdialog.h"

class StartupKTFDialog : public AbstractStartupDialog
{
    Q_OBJECT

public:
    explicit StartupKTFDialog(QWidget *parent = nullptr);
    ~StartupKTFDialog();

    // void GetCorBd() override;

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

    // void FillCor() override;
    // void FillBackCor() override;

    void SetupUI() override;
    void FillBd(QWidget *parent, QString Name, QString Value);

signals:

public slots:

    // void WriteCor() override;
    // void SetupCor() override;
    // void ResetCor() override;
    void SaveToFile() override;
    void ReadFromFile() override;
    bool checkStartupValues() override;

private slots:
};

#endif // CORDIALOGKTF_H
