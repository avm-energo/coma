#ifndef CORDIALOG_H
#define CORDIALOG_H

#include <QDialog>
#include <QByteArray>
#include "../gen/mainwindow.h"
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

    struct information
    {
        quint16 adr;
        int size;
    };

    typedef struct
    {
       float flVal;
       int SigAdr;
    }ModBusSignal;



    void FillCor();
    void FillBackCor();
    float ToFloat(QString text);
    void SetupUI();
    void FillBd(QWidget *parent, QString Name, QString Value);

signals:
   void sendCom45(quint32*);
   void sendCom50(quint32* adr, float *data);
   void RS485WriteCorBd(information*, float*);
   void RS485ReadCorBd(information*);
   void CorReadRequest();
   void WritePasswordChecked();


public slots:
    void GetCorBd(int index);
    void GetCorBdButton();
    void WriteCorBd();
    void WriteCor();
    void WriteCorCapacity();
    void WriteCorTg();
    void MessageOk();
    void SetCor();
    void ResetCor();
    void UpdateFlCorData(Parse104::FlSignals104 *Signal);
    void ModBusUpdateCorData(ModBusSignal *Signal, int * size);
    void SaveToFile();
    void ReadFromFile();
    void WritePasswordCheck(QString psw);
    void TimerTimeout();
    void WriteCorMessageOk();
    void ErrorRead();

private slots:

};

#endif // CORDIALOG_H
