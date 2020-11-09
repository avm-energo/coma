#ifndef ABSTRACTCORDIALOG_H
#define ABSTRACTCORDIALOG_H

#include "../gen/error.h"
#include "../widgets/udialog.h"
//#include "../iec104/iec104.h"
#include "../interfaces/modbus.h"
#include "../models/etablemodel.h"

#include <QByteArray>

class AbstractStartupDialog : public UDialog
{
    Q_OBJECT
public:
    enum UpdateStates
    {
        ThereWasNoUpdatesRecently = 0,
        QueryWasInitiated = 1,
        AnswerWasReceived = 2
    };

    struct StartupBlockStruct
    {
        quint32 initStartRegAdr;
        int num;
        void *block;
        quint32 size;
    };

    explicit AbstractStartupDialog(QWidget *parent = nullptr);
    //    ~AbstractCorDialog();
    void SetStartupBlock(int blocknum, void *block, quint32 blocksize, quint32 startAdr);
    //    QTimer *MessageTimer;

    //    int corDIndex;
    //    bool ok;
    //    int first;

    QWidget *buttonWidget();
    Error::Msg WriteCheckPassword();
    virtual void GetCorBd();
    //    void ETHUpdate();
    //    void MBSUpdate();

private:
    UpdateStates m_updateState;
    StartupBlockStruct m_startupBlockDescription;

    virtual void FillCor() = 0;
    virtual void FillBackCor() = 0;
    //    virtual void FillWb7() = 0;
    //    virtual void FillBackBd9() = 0;
    float ToFloat(QString text);
    virtual void SetupUI() = 0;
    void FillBd(QWidget *parent, QString Name, QString Value);
    void FillBd(QWidget *parent, QString Name, float Value);

signals:
    //    void SendCom45(quint32);
    //    void SendCom50(quint32 adr, float data);
    //    void RS485WriteCorBd(ModBus::Information, float *);
    //    void RS485ReadCorBd(ModBus::Information);
    //    void CorReadRequest();
    //    void WritePasswordChecked();

public slots:
    void GetCorBdButton();
    virtual void WriteCorBd() = 0;
    virtual void WriteCor() = 0;
    //    void MessageOk();
    void SetCor();
    virtual void ResetCor() = 0;
    void updateFloatData();
    void updateStatus();
    //    void ModBusUpdateCorData(QList<ModBus::SignalStruct> Signal);
    virtual void SaveToFile() = 0;
    virtual void ReadFromFile() = 0;
    //    void WritePasswordCheck(QString psw);
    //    void TimerTimeout();
    void ErrorRead();
    void reqUpdate() override;
};

#endif // ABSTRACTCORDIALOG_H
