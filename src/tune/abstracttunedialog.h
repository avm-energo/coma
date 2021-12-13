#ifndef ABSTRACTTUNEDIALOG_H
#define ABSTRACTTUNEDIALOG_H

#include "../datablocks/datablock.h"
#include "../models/report.h"

#include <QByteArray>
#include <QCloseEvent>
#include <QDialog>

#define MAXTUNESIZE 1024 // максимальный размер файла с данными настройки

#define TD_TMK 25.0       // degrees
#define TD_VBAT 3.0       // voltage
#define TD_FREQ 50        // Hz
#define MEASTIMERINT 1000 // интервал проведения измерений - 1 с

#define TUNE_POINTSPER 500 // столько миллисекунд должно усредняться при регулировке
#define WAITFORCONST 1 // seconds to let voltages be constant

#define DEBUGISON

// disable all limits checks
// #define NO_LIMITS
class ConfigV;

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

    explicit AbstractTuneDialog(ConfigV *config, int tuneStep, QWidget *parent = nullptr);
    ~AbstractTuneDialog();

    bool IsNeededDefConf;
    int m_blockCount;
    QStringList m_messages;
    quint32 SecondsToEnd15SecondsInterval;
    QList<Error::Msg (AbstractTuneDialog::*)()> m_tuneFunctions;
    quint8 bStep;
    int TuneVariant; // вариант регулировочных параметров
    // ReportModel *RepModel; // модель, в которую заносим данные для отчёта
    int m_tuneStep;
    bool m_finished;

    void SetupUI();
    QWidget *TuneUI();
    QWidget *MainUI();
    QWidget *BottomUI();
    void WaitNSeconds(int SecondsToWait, bool isAllowedToStop = false);
    void Wait15Seconds();
    void startWait();
    void stopWait();
    Error::Msg CheckPassword();
    virtual void setMessages() = 0;      // заполнить список сообщений
    virtual void setTuneFunctions() = 0; // заполнить список функций настройки
    int addWidgetToTabWidget(QWidget *w, const QString &caption);
    void MsgSetVisible(MsgTypes type, int msg, bool Visible = true);
    void MsgClear();
    void SetBac(DataBlock *block);
    Error::Msg checkCalibrStep();
    //    void saveTuneSequenceFile(int step);
    Error::Msg saveWorkConfig();
    Error::Msg loadWorkConfig();
    Error::Msg saveAllTuneCoefs();
    Error::Msg loadAllTuneCoefs();
    Error::Msg writeTuneCoefs(bool isUserChoosingRequired = true);
    Error::Msg readTuneCoefs();

private:
    QMap<int, DataBlock *> AbsBac;
    struct MainWidgetStruct
    {
        QWidget *widget;
        QString caption;
    };
    QList<MainWidgetStruct> m_mainWidgetList;

    void readTuneCoefsByBac(int bacnum);

signals:
    void Finished();
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

    ConfigV *configV;
};

#endif // ABSTRACTTUNEDIALOG_H
