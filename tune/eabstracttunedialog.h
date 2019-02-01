#ifndef EABSTRACTTUNEDIALOG_H
#define EABSTRACTTUNEDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QByteArray>
#include "../gen/s2.h"
#include "../gen/report.h"

#define MAXTUNESIZE 1024 // максимальный размер файла с данными настройки

#define TD_TMK  25.0 // degrees
#define TD_VBAT 3.0 // voltage
#define TD_FREQ 50 // Hz
#define MEASTIMERINT    1000 // интервал проведения измерений - 1 с

#define TUNE_POINTSPER  500 // столько миллисекунд должно усредняться при регулировке
#define WAITFORCONST    4 // seconds to let voltages be constant

class EAbstractTuneDialog : public QDialog
{
    Q_OBJECT
public:
    struct BacStruct
    {
        void *BacBlock;
        int BacBlockSize;
//        char BacBlockNum;
    };

    explicit EAbstractTuneDialog(QWidget *parent = nullptr);
    ~EAbstractTuneDialog();

    bool IsNeededDefConf;
    QMap <int, BacStruct> AbsBac;
    QStringList lbls;
    bool Cancelled, Skipped, MeasurementEnabled, ok, TuneFileSaved;
    QTimer *MeasurementTimer;
    QVector<S2::DataRec> S2Config;
    quint32 SecondsToEnd15SecondsInterval;
    QHash <QString, int (EAbstractTuneDialog::*)()> pf;
    quint8 bStep;
    int TuneVariant; // вариант регулировочных параметров
    QVector<S2::DataRec> *S2ConfigForTune;
    ReportModel *RepModel; // модель, в которую заносим данные для отчёта
//    QString OrganizationString; // наименование организации, работающей с программой

    virtual void SetupUI() = 0;
    QWidget *TuneUI();
    QWidget *BottomUI(int bacnum);
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
    bool WriteTuneCoefs(int bacnum);
    int SaveAllTuneCoefs();
    void PrereadConf();
    virtual void GetBdAndFillMTT() = 0;
#endif
    virtual void FillBac(int bacnum) = 0;
    virtual void FillBackBac(int bacnum) = 0;
    void SaveToFileEx(int bacnum);
    void ShowTable();

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
    void ReadAllTuneCoefs();
    void ReadTuneCoefs(int bacnum);
    bool WriteTuneCoefsSlot(int bacnum);
    void Good();
    void NoGood();

#endif
    void SaveToFile(int bacnum);

private:
    void SetMeasurementEnabled(bool enabled);

private slots:
#if PROGSIZE != PROGSIZE_EMUL
    void StartTune();
    void PasswordCheck(QString psw);
    virtual int ReadAnalogMeasurements() = 0;
    //    void UpdateNSecondsWidget();
    void MeasTimerTimeout(); // по событию от таймера при активном режиме измерений обновить данные
#endif
    void LoadFromFile(int bacnum);
    virtual void SetDefCoefs(int bacnum) = 0;

protected:
    void closeEvent(QCloseEvent *e);
    void keyPressEvent(QKeyEvent *e);
};

#endif // EABSTRACTTUNEDIALOG_H
