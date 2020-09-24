#ifndef EABSTRACTTUNEDIALOG_H
#define EABSTRACTTUNEDIALOG_H

#include "../gen/report.h"
#include "../gen/s2.h"
#include "../models/valuemodel.h"
#include <QByteArray>
#include <QCloseEvent>
#include <QDialog>

#define MAXTUNESIZE 1024 // максимальный размер файла с данными настройки

#define TD_TMK 25.0 // degrees
#define TD_VBAT 3.0 // voltage
#define TD_FREQ 50 // Hz
#define MEASTIMERINT 1000 // интервал проведения измерений - 1 с

#define TUNE_POINTSPER 500 // столько миллисекунд должно усредняться при регулировке
#define WAITFORCONST 1 // seconds to let voltages be constant

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
    QMap<int, BacStruct> AbsBac;
    QStringList lbls;
    bool Skipped, MeasurementEnabled, ok, TuneFileSaved;
    //    bool Cancelled;
    QTimer *MeasurementTimer;
    QVector<S2::DataRec> S2Config;
    quint32 SecondsToEnd15SecondsInterval;
    QHash<QString, int (EAbstractTuneDialog::*)()> pf;
    quint8 bStep;
    int TuneVariant; // вариант регулировочных параметров
    QVector<S2::DataRec> *S2ConfigForTune;
    ReportModel *RepModel; // модель, в которую заносим данные для отчёта
    //    QString OrganizationString; // наименование организации, работающей с программой
    int TuneIndex, CheckIndex, TimeIndex;
    ValueModel *m_VModel;

    virtual void SetupUI() = 0;
    QWidget *TuneUI();
    QWidget *BottomUI(int bacnum);
    void SetBac(void *block, int blocknum, int blocksize); // установка указателя на блок Bac
    void WaitNSeconds(int SecondsToWait, bool isAllowedToStop = false);
    void ProcessTune();
    int CheckPassword();
    virtual void SetLbls() = 0; // заполнить список сообщений
    virtual void SetPf() = 0; // заполнить список функций настройки
    bool IsWithinLimits(double number, double base, double threshold);
    void MsgSetVisible(int msg, bool Visible = true);
    void OkMsgSetVisible(int msg, bool Visible = true);
    void ErMsgSetVisible(int msg, bool Visible = true);
    void SkMsgSetVisible(int msg, bool Visible = true);
    void MsgClear();
    //    QByteArray *ChooseFileForOpen(QString mask);
    bool WriteTuneCoefs(int bacnum);
    int SaveAllTuneCoefs();
    void PrereadConf();
    virtual void GetBdAndFill() = 0;
    virtual void FillBac(int bacnum) = 0;
    virtual void FillBackBac(int bacnum) = 0;
    void SaveToFileEx(int bacnum);
    void ShowTable();
    void ReadTuneCoefsByBac(int bacnum);

signals:
    void PasswordChecked();
    void stopall();
    void dataready(QByteArray);
    //    void SecondsRemaining(quint32);
    void Finished();
    void LoadDefConf();
    void stopRead(int);

public slots:
    void CancelTune();
    void ReadAllTuneCoefs();
    void ReadTuneCoefs();
    bool WriteTuneCoefsSlot();
    void Good();
    void NoGood();
    int StartMeasurement();
    virtual void SetDefCoefs() = 0;
    void TuneReadCoefs(int);

    void SaveToFile();

private:
    void SetMeasurementEnabled(bool enabled);

private slots:
    void StartTune();
    void PasswordCheck(QString psw);
    virtual int ReadAnalogMeasurements() = 0;
    //    void UpdateNSecondsWidget();
    void MeasTimerTimeout(); // по событию от таймера при активном режиме измерений обновить данные
    void LoadFromFile();

protected:
    void closeEvent(QCloseEvent *e);
    void keyPressEvent(QKeyEvent *e);
};

#endif // EABSTRACTTUNEDIALOG_H
