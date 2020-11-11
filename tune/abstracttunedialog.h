#ifndef ABSTRACTTUNEDIALOG_H
#define ABSTRACTTUNEDIALOG_H

#include "../gen/datablock.h"
#include "../gen/report.h"
#include "../gen/s2.h"
#include "../gen/udialog.h"

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

    explicit AbstractTuneDialog(QWidget *parent = nullptr);
    ~AbstractTuneDialog();

    bool IsNeededDefConf;

    //    int m_ConfigCounter;
    //    QMap<int, BlockStruct> m_TuneBlockMap;
    //    QMap<int, BlockStruct> m_ConfigBlockMap;
    QMap<int, DataBlock *> m_blocks;
    int m_blockCount;

    QStringList lbls;
    bool Skipped, MeasurementEnabled, ok, TuneFileSaved;
    //    bool Cancelled;
    QTimer *MeasurementTimer;
    S2ConfigType *S2Config;
    quint32 SecondsToEnd15SecondsInterval;
    QHash<QString, Error::Msg (AbstractTuneDialog::*)()> pf;
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
    int addDataBlock(DataBlock::DataBlockTypes type, const QString &caption, void *block, void *defblock, int blocknum,
        int blocksize);
    //    int setConfigPtr(void *ptr, int size);

    void WaitNSeconds(int SecondsToWait, bool isAllowedToStop = false);
    void Wait15Seconds();

    //    void ProcessTune();
    Error::Msg CheckPassword();
    virtual void SetLbls() = 0; // заполнить список сообщений
    virtual void SetPf() = 0;   // заполнить список функций настройки
                                //    bool IsWithinLimits(double number, double base, double threshold);
    void MsgSetVisible(int msg, bool Visible = true);
    void OkMsgSetVisible(int msg, bool Visible = true);
    void ErMsgSetVisible(int msg, bool Visible = true);
    void SkMsgSetVisible(int msg, bool Visible = true);
    void MsgClear();
    //    QByteArray *ChooseFileForOpen(QString mask);
    bool WriteTuneCoefs(int bacnum);
    Error::Msg SaveAllTuneCoefs();
    void PrereadConf();
    virtual void GetBdAndFill() = 0;
    virtual void FillBac(int bacnum) = 0;
    virtual void FillBackBac(int bacnum) = 0;
    void SaveToFileEx(int bacnum);
    //    void ShowTable();
    void ReadBlocks(DataBlock::DataBlockTypes type);
    Error::Msg LoadTuneSequenceFile();
    Error::Msg CheckCalibrStep();
    void SaveTuneSequenceFile();
    virtual Error::Msg SaveWorkConfig(int configblocknum = 0);

signals:
    //    void PasswordChecked();
    void stopall();
    void dataready(QByteArray);
    //    void SecondsRemaining(quint32);
    void Finished();
    void LoadDefConf();
    void stopRead(int);

public slots:
    void CancelTune();
    //    void ReadAllTuneCoefs();
    void ReadTuneCoefs();
    bool WriteTuneCoefsSlot();
    void Good();
    void NoGood();
    Error::Msg StartMeasurement();
    virtual void SetDefCoefs() = 0;
    void TuneReadCoefs(int);

    void SaveToFile();

private:
    void SetMeasurementEnabled(bool enabled);

private slots:
    void StartTune();
    //    void PasswordCheck(QString psw);
    virtual int ReadAnalogMeasurements() = 0;
    //    void UpdateNSecondsWidget();
    void MeasTimerTimeout(); // по событию от таймера при активном режиме измерений обновить данные
    void LoadFromFile();

protected:
    void closeEvent(QCloseEvent *e);
    void keyPressEvent(QKeyEvent *e);
};

#endif // ABSTRACTTUNEDIALOG_H
