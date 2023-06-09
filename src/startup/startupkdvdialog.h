#ifndef CORDIALOGKDV_H
#define CORDIALOGKDV_H

#include "abstractstartupdialog.h"

class StartupKDVDialog : public AbstractStartupDialog
{
    Q_OBJECT
public:
    StartupKDVDialog(QWidget *parent = nullptr);
    ~StartupKDVDialog();

    void GetCorBd() override;

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

    void FillBackCor() override;
    void FillCor() override;
    void FillBackWBd8();
    void SetupUI() override;
    void FillBd(QWidget *parent, QString Name, QString Value);

public slots:
    void WriteCor() override;
    void SetupCor() override;
    void ResetCor() override;
    void SaveToFile() override;
    void ReadFromFile() override;
};

#endif // CORDIALOGKDV_H
