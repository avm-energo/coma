#ifndef CORDIALOG_H
#define CORDIALOG_H

#include <QDialog>
#include <QByteArray>
#include "../modbus/modbus.h"
#include "../iec104/iec104.h"
#include "../widgets/etablemodel.h"

class CorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CorDialog(QWidget *parent = nullptr);
    ~CorDialog();

    int corDIndex;
    bool ok;
    int first;
    QTimer *MessageTimer;

    int WriteCheckPassword();
    void GetCorBd(int index);

private:
    struct CorData
    {
        float C_init[3]; //Начальные значени емкостей вводов
        float Tg_init[3]; //Начальные значения tgδ вводов
        float corTg[3];	// коррекция  tg δ вводов
        float Iunb_init;		// Начальное действ. значение тока небаланса
        float Phy_unb_init;	// Начальное значение угла тока небаланса
    };

    CorData *CorBlock;

    void FillCor();
    void FillBackCor();
    float ToFloat(QString text);
    void SetupUI();
    void FillBd(QWidget *parent, QString Name, QString Value);

signals:
   void sendCom45(quint32);
   void sendCom50(quint32 adr, float data);
   void RS485WriteCorBd(ModBus::Information, float *);
   void RS485ReadCorBd(ModBus::Information);
   void CorReadRequest();
   void WritePasswordChecked();


public slots:
    void GetCorBdButton();
    void WriteCorBd();
    void WriteCor();
    void MessageOk();
    void SetCor();
    void ResetCor();
    void UpdateFlCorData(Parse104::FlSignals104 *Signal);
    void ModBusUpdateCorData(QList<ModBus::SignalStruct> Signal);
//    void ModbusCorDataWritten();
    void SaveToFile();
    void ReadFromFile();
    void WritePasswordCheck(QString psw);
    void TimerTimeout();
//    void WriteCorMessageOk();
    void ErrorRead();

private slots:

};

#endif // CORDIALOG_H
