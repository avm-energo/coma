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

    // void GetCorBd() override;

private:
    bool WriteCheckPassword();
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

    void FillCor() override;
    void FillBackCor() override;
    //    float ToFloat(QString text);
    void SetupUI() override;
    // void FillBd(QWidget *parent, QString Name, QString Value);

signals:

public slots:
    //    void GetCorBdButton() override;
    void WriteCorBd() override;
    void WriteCor() override;
    void MessageOk();
    void SetCor();
    void ResetCor() override;
    // void UpdateFlCorData(IEC104Thread::FlSignals104 *Signal);
    // void ModBusUpdateCorData(QList<ModBus::SignalStruct> Signal);
    void SaveToFile() override;
    void ReadFromFile() override;
    //    void WritePasswordCheck(QString psw);
    //    void TimerTimeout();
    void ErrorRead();

private slots:
};

#endif // CORDIALOG_H
