#ifndef EABSTRACTTUNEDIALOG_H
#define EABSTRACTTUNEDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QByteArray>
#include <QStringList>
#include "../gen/publicclass.h"

#define MAXTUNESIZE 1024 // максимальный размер файла с данными настройки

#define TD_TMK  25.0 // degrees
#define TD_VBAT 3.0 // voltage
#define TD_FREQ 50 // Hz
#define MEASTIMERINT    1000 // интервал проведения измерений - 1 с

#define TUNE_POINTSPER  500 // столько миллисекунд должно усредняться при регулировке

class EAbstractTuneDialog : public QDialog
{
    Q_OBJECT
public:
    struct BacStruct
    {
        void *BacBlock;
        int BacBlockSize;
        int BacBlockNum;
    };

    explicit EAbstractTuneDialog(QWidget *parent = 0);

    BacStruct AbsBac;
    QStringList lbls;
    bool Skipped, MeasurementEnabled, ok, TuneFileSaved;
    QTimer *MeasurementTimer;
    QVector<publicclass::DataRec> S2Config;
    quint32 SecondsToEnd15SecondsInterval;
    QHash <QString, int (EAbstractTuneDialog::*)()> pf;
    quint8 bStep;
    int TuneVariant; // вариант регулировочных параметров

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
//    QByteArray *ChooseFileForOpen(QString mask);
    void InputTuneVariant(int varnum);
    bool WriteTuneCoefs();

signals:
    void PasswordChecked();
    void stopall();
    void dataready(QByteArray);
    void SecondsRemaining(quint32);
    void Finished();

public slots:
    void CancelTune();
    void ReadTuneCoefs();
    bool WriteTuneCoefsSlot();
    void SaveToFile();
    void Good();
    void NoGood();

private:

private slots:
    void StartTune();
    void PasswordCheck(QString &psw);
    void LoadFromFile();
    virtual int ReadAnalogMeasurements() = 0;
    virtual void SetDefCoefs() = 0;
    void UpdateNSecondsWidget();
    void MeasTimerTimeout(); // по событию от таймера при активном режиме измерений обновить данные
    void SetTuneVariant();

protected:
    void closeEvent(QCloseEvent *e);
    void keyPressEvent(QKeyEvent *e);
};

#endif // EABSTRACTTUNEDIALOG_H
