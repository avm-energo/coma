#ifndef EABSTRACTTUNEDIALOG_H
#define EABSTRACTTUNEDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QByteArray>
#include "../gen/s2.h"
//#include "../gen/publicclass.h"

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

    explicit EAbstractTuneDialog(QWidget *parent = nullptr);
    ~EAbstractTuneDialog();

    bool IsNeededDefConf = false;
    BacStruct AbsBac;
    QStringList lbls;
    bool Skipped, MeasurementEnabled, ok, TuneFileSaved;
    QTimer *MeasurementTimer;
    QVector<S2::DataRec> S2Config;
    quint32 SecondsToEnd15SecondsInterval;
    QHash <QString, int (EAbstractTuneDialog::*)()> pf;
    quint8 bStep;
    bool Cancelled;
    int TuneVariant; // вариант регулировочных параметров
     QVector<S2::DataRec> *S2ConfigForTune;

    virtual void SetupUI() = 0;
    QWidget *TuneUI();
    QWidget *BottomUI();
    void SetBac(void *block, int blocknum, int blocksize); // установка указателя на блок Bac
#if PROGSIZE != PROGSIZE_EMUL
    void ProcessTune();
    int CheckPassword();
    virtual void SetLbls() = 0; // заполнить список сообщений
    virtual void SetPf() = 0; // заполнить список функций настройки
    bool IsWithinLimits(double number, double base, double threshold);
    void MsgSetVisible(int msg, bool Visible=true);
    void OkMsgSetVisible(int msg, bool Visible=true);
    void ErMsgSetVisible(int msg, bool Visible=true);
    void SkMsgSetVisible(int msg, bool Visible=true);
    void MsgClear();
    void WaitNSeconds(int SecondsToWait, bool isAllowedToStop=false);
    int StartMeasurement();
//    QByteArray *ChooseFileForOpen(QString mask);
    void InputTuneVariant(int varnum);
    bool WriteTuneCoefs();
    void PrereadConf();
    virtual void GetBdAndFillMTT() = 0;
#endif
    virtual void FillBac() = 0;
    virtual void FillBackBac() = 0;
    void SaveToFileEx();

signals:
    void PasswordChecked();
    void stopall();
    void dataready(QByteArray);
//    void SecondsRemaining(quint32);
    void Finished();
    void LoadDefConf();

public slots:
#if PROGSIZE != PROGSIZE_EMUL
    void CancelTune();
    void ReadTuneCoefs();
    bool WriteTuneCoefsSlot();
    void Good();
    void NoGood();
#endif
    void SaveToFile();

private:

private slots:
#if PROGSIZE != PROGSIZE_EMUL
    void StartTune();
    void PasswordCheck(QString psw);
    virtual int ReadAnalogMeasurements() = 0;
    //    void UpdateNSecondsWidget();
        void MeasTimerTimeout(); // по событию от таймера при активном режиме измерений обновить данные
        void SetTuneVariant();
#endif
    void LoadFromFile();
    virtual void SetDefCoefs() = 0;

protected:
    void closeEvent(QCloseEvent *e);
    void keyPressEvent(QKeyEvent *e);
};

#endif // EABSTRACTTUNEDIALOG_H
