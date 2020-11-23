#ifndef CORDIALOG_H
#define CORDIALOG_H

//#include "../iec104/iec104.h"
//#include "../modbus/modbus.h"
#include "abstractstartupdialog.h"

#include <QByteArray>
#include <QDialog>

#define KIVSTARTUPINITREG 4000

class StartupKIVDialog : public AbstractStartupDialog
{
    Q_OBJECT

public:
    explicit StartupKIVDialog(QWidget *parent = nullptr);
    ~StartupKIVDialog();

    bool WriteCheckPassword();
    void GetCorBd() override;

private:
    struct CorData
    {
        float C_init[3];    //Начальные значени емкостей вводов
        float Tg_init[3];   //Начальные значения tgδ вводов
        float corTg[3];     // коррекция  tg δ вводов
        float Iunb_init;    // Начальное действ. значение тока небаланса
        float Phy_unb_init; // Начальное значение угла тока небаланса
    };

    CorData *CorBlock;

    void FillCor() override;
    void FillBackCor() override;
    //    float ToFloat(QString text);
    void SetupUI() override;
    // void FillBd(QWidget *parent, QString Name, QString Value);

signals:
    //    void SendCom45(quint32);
    //    void SendCom50(quint32 adr, float data);
    //    void RS485WriteCorBd(ModBus::Information, float *);
    //    void RS485ReadCorBd(ModBus::Information);
    //    void CorReadRequest();
    //    void WritePasswordChecked();

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
