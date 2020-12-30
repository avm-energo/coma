#ifndef ABSTRACTTUNEDIALOG_H
#define ABSTRACTTUNEDIALOG_H

#include "../datablocks/datablock.h"
#include "../gen/report.h"
#include "../gen/s2.h"
//#include "../widgets/udialog.h"

#include <QByteArray>
#include <QCloseEvent>

#define MAXTUNESIZE 1024 // максимальный размер файла с данными настройки

#define TD_TMK 25.0       // degrees
#define TD_VBAT 3.0       // voltage
#define TD_FREQ 50        // Hz
#define MEASTIMERINT 1000 // интервал проведения измерений - 1 с

#define TUNE_POINTSPER 500 // столько миллисекунд должно усредняться при регулировке
#define WAITFORCONST 1 // seconds to let voltages be constant

class AbstractTuneDialog : public QDialog
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
    bool m_finished;

    void SetupUI();
    QWidget *TuneUI();
    QWidget *MainUI();
    QWidget *BottomUI();
    //    virtual QWidget *MainUI() = 0;
    //    int setConfigPtr(void *ptr, int size);

    void WaitNSeconds(int SecondsToWait, bool isAllowedToStop = false);
    void Wait15Seconds();
    void startWait();
    void stopWait();

    //    void ProcessTune();
    Error::Msg CheckPassword();
    virtual void setMessages() = 0;      // заполнить список сообщений
    virtual void setTuneFunctions() = 0; // заполнить список функций настройки
    int addWidgetToTabWidget(QWidget *w, const QString &caption);
    //    void MsgSetVisible(int msg, bool Visible = true);
    void MsgSetVisible(MsgTypes type, int msg, bool Visible = true);
    //    void OkMsgSetVisible(int msg, bool Visible = true);
    //    void ErMsgSetVisible(int msg, bool Visible = true);
    //    void SkMsgSetVisible(int msg, bool Visible = true);
    void MsgClear();
    //    QByteArray *ChooseFileForOpen(QString mask);
    //    bool WriteTuneCoefs(int blocknum);
    //    void SaveTuneBlocksToFiles();
    //    void PrereadConf();
    //    void GetBdAndFill();

    //    void SetBac(void *block, int blocknum, int blocksize);
    void SetBac(DataBlock *block);
    //    virtual void FillBac(int bacnum) = 0;
    //    virtual void FillBackBac(int bacnum) = 0;
    //    void SaveToFileEx(int bacnum);
    //    void ShowTable();
    //    Error::Msg LoadTuneSequenceFile();
    Error::Msg checkCalibrStep();
    void saveTuneSequenceFile(int step);
    Error::Msg saveWorkConfig();
    Error::Msg loadWorkConfig();
    Error::Msg saveAllTuneCoefs();
    Error::Msg loadAllTuneCoefs();
    bool writeTuneCoefs();
    Error::Msg readTuneCoefs();
    //    void scrollArea();

private:
    //    struct BlockStruct
    //    {
    //        void *BacBlock;
    //        int BacBlockSize;
    //        //        char BacBlockNum;
    //    };

    //    QMap<int, BlockStruct> AbsBac;
    QMap<int, DataBlock *> AbsBac;
    struct MainWidgetStruct
    {
        QWidget *widget;
        QString caption;
    };
    QList<MainWidgetStruct> m_mainWidgetList;

    //    BlockStruct InitialBci;

    void readTuneCoefsByBac(int bacnum);
    Error::Msg writeTuneCoefsByBac(int bacnum);

signals:
    //    void PasswordChecked();
    //    void stopall();
    //    void dataready(QByteArray);
    //    void SecondsRemaining(quint32);
    void Finished();
    void LoadDefConf();
    //    void stopRead(int);
    void setProgressSize(quint64 size);
    void setProgressCount(quint64 count);

public slots:
    void showTWTab(int index);
    void CancelTune();
    //    void ReadAllTuneCoefs();
    void writeTuneCoefsSlot();
    void loadTuneCoefsSlot();
    void saveTuneCoefsSlot();
    //    void Good();
    //    void NoGood();
    //    Error::Msg StartMeasurement();
    //    virtual void setDefCoefs() = 0;
    //    void TuneReadCoefs(int);

    //    void SaveToFile();

    // private:
    //    void SetMeasurementEnabled(bool enabled);

private slots:
    void startTune();
    //    void closeThis();
    //    void PasswordCheck(QString psw);
    //    virtual int ReadAnalogMeasurements() = 0;
    //    void UpdateNSecondsWidget();
    //    void MeasTimerTimeout(); // по событию от таймера при активном режиме измерений обновить данные
    //    void LoadTuneBlocksFromFile();

protected:
    void closeEvent(QCloseEvent *e);
    void keyPressEvent(QKeyEvent *e);
};

#endif // ABSTRACTTUNEDIALOG_H
