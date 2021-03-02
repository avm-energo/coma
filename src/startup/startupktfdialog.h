#ifndef CORDIALOGKTF_H
#define CORDIALOGKTF_H

#include "abstractstartupdialog.h"

class StartupKTFDialog : public AbstractStartupDialog
{
    Q_OBJECT

public:
    explicit StartupKTFDialog(QWidget *parent = nullptr);
    ~StartupKTFDialog();

    //    int corDIndex;
    //    bool ok;
    //    int first;
    //    QTimer *MessageTimer;

    // int WriteCheckPassword();
    void GetCorBd() override;

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

    void FillCor() override;
    void FillBackCor() override;
    //    float ToFloat(QString text);
    void SetupUI() override;
    void FillBd(QWidget *parent, QString Name, QString Value);

signals:

public slots:
    //    void GetCorBdButton() override;
    void WriteCor() override;
    void SetupCor() override;
    void ResetCor() override;
    void SaveToFile() override;
    void ReadFromFile() override;

private slots:
};

#endif // CORDIALOGKTF_H
