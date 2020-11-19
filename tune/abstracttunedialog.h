#ifndef ABSTRACTTUNEDIALOG_H
#define ABSTRACTTUNEDIALOG_H

#include "../gen/report.h"
#include "../gen/s2.h"
#include "../widgets/udialog.h"

#include <QByteArray>
#include <QCloseEvent>

#define MAXTUNESIZE 1024 // максимальный размер файла с данными настройки

#define TD_TMK 25.0       // degrees
#define TD_VBAT 3.0       // voltage
#define TD_FREQ 50        // Hz
#define MEASTIMERINT 1000 // интервал проведения измерений - 1 с

#define TUNE_POINTSPER 500 // столько миллисекунд должно усредняться при регулировке
#define WAITFORCONST 1 // seconds to let voltages be constant

class AbstractTuneDialog : public UDialog
{
    Q_OBJECT
public:
    //    struct BlockStruct
    //    {
    //        void *block;
    //        int blocksize;
    //    };
    enum MsgTypes
    {
        OkMsg,
        ErMsg,
        SkMsg,
        NoMsg
    };

    explicit AbstractTuneDialog(int tuneStep, QWidget *parent = nullptr);
    ~AbstractTuneDialog();

    bool IsNeededDefConf;

    //    int m_ConfigCounter;
    //    QMap<int, BlockStruct> m_TuneBlockMap;
    //    QMap<int, BlockStruct> m_ConfigBlockMap;
    int m_blockCount;

    QStringList m_messages;
    //    bool Skipped, MeasurementEnabled; //, ok; //, TuneFileSaved;
    //    bool Cancelled;
    QTimer *MeasurementTimer;
    //    S2ConfigType *S2Config;
    quint32 SecondsToEnd15SecondsInterval;
    QHash<QString, Error::Msg (AbstractTuneDialog::*)()> m_tuneFunctions;
    quint8 bStep;
    int TuneVariant;       // вариант регулировочных параметров
    ReportModel *RepModel; // модель, в которую заносим данные для отчёта
    //    QString OrganizationString; // наименование организации, работающей с программой
    //    ValueModel *m_VModel;
    int m_tuneStep;

    void SetupUI();
    QWidget *TuneUI();
    QWidget *BottomUI();
    virtual QWidget *MainUI() = 0;
    //    int setConfigPtr(void *ptr, int size);

    void WaitNSeconds(int SecondsToWait, bool isAllowedToStop = false);
    void Wait15Seconds();

    //    void ProcessTune();
    Error::Msg CheckPassword();
    virtual void setMessages() = 0;      // заполнить список сообщений
    virtual void setTuneFunctions() = 0; // заполнить список функций настройки
                                         //    bool IsWithinLimits(double number, double base, double threshold);
                                         //    void MsgSetVisible(int msg, bool Visible = true);
    void MsgSetVisible(MsgTypes type, int msg, bool Visible = true);
    //    void OkMsgSetVisible(int msg, bool Visible = true);
    //    void ErMsgSetVisible(int msg, bool Visible = true);
    //    void SkMsgSetVisible(int msg, bool Visible = true);
    void MsgClear();
    //    QByteArray *ChooseFileForOpen(QString mask);
    bool WriteTuneCoefs(int blocknum);
    void SaveTuneBlocksToFiles();
    void PrereadConf();
    //    void GetBdAndFill();

    virtual void FillBac(int bacnum) = 0;
    virtual void FillBackBac(int bacnum) = 0;
    //    void SaveToFileEx(int bacnum);
    //    void ShowTable();
    Error::Msg LoadTuneSequenceFile();
    Error::Msg checkCalibrStep();
    void saveTuneSequenceFile();
    Error::Msg saveWorkConfig();
    Error::Msg loadWorkConfig();

private:
    struct BacStruct
    {
        void *BacBlock;
        int BacBlockSize;
        //        char BacBlockNum;
    };

    QMap<int, BacStruct> AbsBac;

    void SetBac(void *block, int blocknum, int blocksize);

    int SaveAllTuneCoefs();
    void ReadTuneCoefsByBac(int bacnum);
signals:
    //    void PasswordChecked();
    //    void stopall();
    //    void dataready(QByteArray);
    //    void SecondsRemaining(quint32);
    void Finished();
    void LoadDefConf();
    //    void stopRead(int);

public slots:
    void CancelTune();
    //    void ReadAllTuneCoefs();
    void readTuneCoefs();
    bool writeTuneCoefs();
    //    void Good();
    //    void NoGood();
    Error::Msg StartMeasurement();
    virtual void SetDefCoefs() = 0;
    //    void TuneReadCoefs(int);

    //    void SaveToFile();

    // private:
    //    void SetMeasurementEnabled(bool enabled);

private slots:
    void StartTune();
    //    void PasswordCheck(QString psw);
    virtual int ReadAnalogMeasurements() = 0;
    //    void UpdateNSecondsWidget();
    void MeasTimerTimeout(); // по событию от таймера при активном режиме измерений обновить данные
    void LoadTuneBlocksFromFile();

protected:
    void closeEvent(QCloseEvent *e);
    void keyPressEvent(QKeyEvent *e);
};

#endif // ABSTRACTTUNEDIALOG_H
