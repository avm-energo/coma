#ifndef CORDIALOG_H
#define CORDIALOG_H

#include "abstractstartupdialog.h"

#define KIVSTARTUPINITREG 4000

class StartupKIVDialog : public AbstractStartupDialog
{
    Q_OBJECT

public:
    explicit StartupKIVDialog(QWidget *parent = nullptr);
    ~StartupKIVDialog();

    void SetupCor() override;

private:
    struct CorData
    {
        float C_init[3];    //Начальные значени емкостей вводов
        float Tg_init[3];   //Начальные значения tgδ вводов
        float corTg[3];     // коррекция  tg δ вводов
        float Iunb_init;    // Начальное действ. значение тока небаланса
        float Phy_unb_init; // Начальное значение угла тока небаланса
                            //   quint32 stat;       // Reserved trash bin
    };

    CorData *CorBlock;

    void SetupUI() override;

signals:

public slots:

    void SaveToFile() override;
    void ReadFromFile() override;

    void ErrorRead();

private slots:
};

#endif // CORDIALOG_H
