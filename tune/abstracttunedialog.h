#ifndef ABSTRACTTUNEDIALOG_H
#define ABSTRACTTUNEDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QByteArray>
#include <QStringList>
#include "../publicclass.h"

#define TD_MEASNUM    4 // количество проводимых измерений для усреднения

#define TD_TMK  25.0 // degrees
#define TD_VBAT 3.0 // voltage
#define TD_FREQ 50 // Hz
#define SLEEPINT    20
#define MEASTIMERINT    1000 // интервал проведения измерений - 1 с

class AbstractTuneDialog : public QDialog
{
    Q_OBJECT
public:
    struct BacStruct
    {
        void *BacBlock;
        int BacBlockSize;
        int BacBlockNum;
    };

    explicit AbstractTuneDialog(QWidget *parent = 0);

    BacStruct AbsBac;
    QStringList lbls;
    bool Cancelled, Skipped, MeasurementEnabled, ok;
    QTimer *MeasurementTimer;
    QVector<publicclass::DataRec> S2Config;
    int SecondsToEnd15SecondsInterval;
    QHash <QString, int (AbstractTuneDialog::*)()> pf;

    virtual void SetupUI() = 0;
    QWidget *TuneUI();
    QWidget *BottomUI();
    void ProcessTune();
    int CheckPassword();
    virtual void SetLbls() = 0; // заполнить список сообщений
    virtual void SetPf() = 0; // заполнить список функций настройки
    void SetBac(void *block, int blocknum, int blocksize); // установка указателя на блок Bac
    virtual void FillBac() = 0;
    virtual void FillBackBac() = 0;
    virtual void GetBdAndFillMTT() = 0;
    bool IsWithinLimits(double number, double base, double threshold);
    void MsgSetVisible(int msg, bool Visible=true);
    void OkMsgSetVisible(int msg, bool Visible=true);
    void ErMsgSetVisible(int msg, bool Visible=true);
    void SkMsgSetVisible(int msg, bool Visible=true);
    void MsgClear();
    void WaitNSeconds(int SecondsToWait);
    void SaveToFileEx();
    int StartMeasurement();
    QByteArray *LoadFile(QString mask);

signals:
    void PasswordChecked();
    void stopall();
    void dataready(QByteArray);
    void SecondsRemaining(QString);

public slots:
    void CancelTune();
    void WriteTuneCoefs();
    void SaveToFile();

private:

private slots:
    void StartTune();
    void PasswordCheck(QString psw);
    void ReadTuneCoefs();
    void LoadFromFile();
    virtual int ReadAnalogMeasurements() = 0;
    void Good();
    void NoGood();
    virtual void SetDefCoefs() = 0;
    void UpdateNSecondsWidget();
    void MeasTimerTimeout(); // по событию от таймера при активном режиме измерений обновить данные

protected:
    void closeEvent(QCloseEvent *e);
};

#endif // ABSTRACTTUNEDIALOG_H
