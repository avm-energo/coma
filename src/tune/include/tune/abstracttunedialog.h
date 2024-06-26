#pragma once

#include <QByteArray>
#include <QCloseEvent>
#include <QDialog>
#include <datablocks/datablock.h>
#include <device/current_device.h>
#include <device/device_list.h>
#include <s2/s2configuration.h>
#include <widgets/tunetabwidget.h>

#define MAXTUNESIZE 1024 // максимальный размер файла с данными настройки

#define TD_TMK 25.0       // degrees
#define TD_VBAT 3.0       // voltage
#define TD_FREQ 50        // Hz
#define MEASTIMERINT 1000 // интервал проведения измерений - 1 с
#define TIMEFORBDATOSETINMS 2000 // время, необходимое для значений Bda, чтобы установиться

#define TUNE_POINTSPER 500 // столько миллисекунд должно усредняться при регулировке
#define WAITFORCONST 1 // seconds to let voltages be constant

//#define DEBUGISON

// disable all limits checks
// #define NO_LIMITS

using ReportData = std::map<QString, QString>;

class AbstractTuneDialog : public QDialog
{
    Q_OBJECT
public:
    enum MsgTypes
    {
        OkMsg,
        ErMsg,
        SkMsg,
        NoMsg
    };

    using TuneFunc = Error::Msg (AbstractTuneDialog::*)();

    struct TuneFuncStruct
    {
        QString message;
        TuneFunc func;
    };

    explicit AbstractTuneDialog(int tuneStep, Device::CurrentDevice *device, QWidget *parent = nullptr);

    template <typename T> //
    void addTuneFunc(const QString &msg, Error::Msg (T::*func)())
    {
        m_tuneFunctions.append({ msg, reinterpret_cast<Error::Msg (AbstractTuneDialog::*)()>(func) });
    }

    bool IsNeededDefConf;
    int m_blockCount;
    QList<TuneFuncStruct> m_tuneFunctions;
    quint8 bStep;
    int TuneVariant; // вариант регулировочных параметров
    // ReportModel *RepModel; // модель, в которую заносим данные для отчёта
    int m_tuneStep;
    bool m_finished;

    void setupUI();
    QWidget *tuneUI();
    QWidget *mainUI();
    QWidget *bottomUI();
    void waitNSeconds(int SecondsToWait, bool isAllowedToStop = false);
    void startWait();
    void stopWait();
    Error::Msg CheckPassword();
    virtual void setTuneFunctions() = 0; // заполнить список функций настройки
    int addWidgetToTabWidget(QWidget *w, const QString &caption);
    void MsgSetVisible(MsgTypes type, int msg, bool visible = true);
    void MsgClear();
    void setBac(DataBlock *block);
    Error::Msg checkCalibrStep();
    //    void saveTuneSequenceFile(int step);
    Error::Msg saveWorkConfig();
    Error::Msg loadWorkConfig();
    Error::Msg saveAllTuneCoefs();
    Error::Msg loadAllTuneCoefs();
    Error::Msg writeTuneCoefs();
    Error::Msg writeTuneCoefs(bool isUserChoosingRequired);
    Error::Msg readTuneCoefs();
    Error::Msg sendChangedConfig(const std::vector<std::pair<QString, S2::valueType>> &changes) const;
    static ReportData &getReportData();

protected:
    Device::CurrentDevice *m_device;
    S2::Configuration &config;
    Interface::AsyncConnection *m_async;
    Interface::SyncConnection *m_sync;
    Device::BaseBoard m_typeB;
    Device::MezzanineBoard m_typeM;
    static ReportData s_reportData;

    Error::Msg setCurrentsTo(const float value);
    static void writeReportData(const QString &name, const QString &value);

private:
    QMap<int, DataBlock *> AbsBac;
    TuneTabWidget *tuneTabWidget;
    void readTuneCoefsByBac(int bacnum);

signals:
    void Finished();
    void cancelled();
    void generalEventReceived();
    void LoadDefConf();
    void setProgressSize(int size);
    void setProgressCount(int count);

public slots:
    void showTWTab(int index);
    void CancelTune();
    void writeTuneCoefsSlot();
    //    void loadTuneCoefsSlot();
    //    void saveTuneCoefsSlot();

private slots:
    void startTune();
    void setProgressSizeSlot(int size);
    void setProgressCountSlot(int count);

protected:
    void closeEvent(QCloseEvent *e);
    void keyPressEvent(QKeyEvent *e);
};

using TuneFunc = Error::Msg (AbstractTuneDialog::*)();
