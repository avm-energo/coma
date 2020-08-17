#ifndef CORDIALOGKTF_H
#define CORDIALOGKTF_H

#include "../dialogs/abstractcordialog.h"
#include "../iec104/iec104.h"
#include "../modbus/modbus.h"
#include "../models/etablemodel.h"

#include <QByteArray>
#include <QDialog>

class CorDialogKTF : public AbstractCorDialog
{
    Q_OBJECT

public:
    explicit CorDialogKTF(QWidget *parent = nullptr);
    ~CorDialogKTF();

    int corDIndex;
    bool ok;
    int first;
    QTimer *MessageTimer;

    // int WriteCheckPassword();
    void GetCorBd(int index);

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

    void FillCor();
    void FillBackCor();
    float ToFloat(QString text);
    void SetupUI();
    void FillBd(QWidget *parent, QString Name, QString Value);

signals:
    void SendCom45(quint32);
    void SendCom50(quint32 adr, float data);
    void RS485WriteCorBd(ModBus::Information, float *);
    void RS485ReadCorBd(ModBus::Information);
    void CorReadRequest();
    //   void WritePasswordChecked();

public slots:
    void GetCorBdButton();
    void WriteCorBd();
    void WriteCor();
    // void MessageOk();
    // void SetCor();
    void ResetCor();
    //  void UpdateFlCorData(IEC104Thread::FlSignals104 *Signal);
    // void ModBusUpdateCorData(QList<ModBus::SignalStruct> Signal);
    void SaveToFile();
    void ReadFromFile();
    //   void WritePasswordCheck(QString psw);
    // void TimerTimeout();
    // void ErrorRead();

private slots:
};

#endif // CORDIALOGKTF_H
