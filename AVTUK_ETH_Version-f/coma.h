#ifndef COMA_H
#define COMA_H

#include "../alarm/alarmstateall.h"
#include "../alarm/alarmwidget.h"
#include "../alarm/avaralarmkiv.h"
#include "../alarm/avaralarmktf.h"
#include "../alarm/warnalarmkiv.h"
#include "../alarm/warnalarmktf.h"
#include "../check/eabstractcheckdialog.h"
#include "../config/abstractconfdialog.h"
#include "../config/confdialog.h"
#include "../dialogs/connectdialog.h"
#include "../dialogs/cordialog.h"
#include "../dialogs/cordialogkdv.h"
#include "../dialogs/cordialogktf.h"
#include "../dialogs/fwupdialog.h"
#include "../dialogs/infodialog.h"
#include "../dialogs/journalsdialog.h"
#include "../dialogs/mnktime.h"
#ifdef AVM_DEBUG
#include "../tune/eabstracttunedialog.h"
#endif
#include "../widgets/etabwidget.h"

#include <QMainWindow>
#include <QResizeEvent>

enum INTERVAL
{
    RECONNECT = 3000,
    WAIT = 300000
};

enum THREAD
{
    USB = 0x01,
    P104 = 0x02,
    MODBUS = 0x04
};

class Coma : public QMainWindow
{
    Q_OBJECT

public:
    enum Modes
    {
        COMA_GENERALMODE, // обычный режим
        COMA_AUTON_OSCMODE, // автономный режим с отображением сохранённой осциллограммы
        COMA_AUTON_PROTMODE, // автономный режим с отображением протокола из прибора
        COMA_AUTON_SWJMODE, // автономный режим с отображением сохранённого журнала
        COMA_AUTON_MODE // просто автономный режим
    };

    typedef struct
    {
        quint32 SigVal;
        quint8 SigQuality;
        // quint64 CP56Time;
    } BS104;

    struct Bd11
    {
        quint32 dev;
        quint32 predAlarm;
        quint32 alarm;
    };

    static QStringList Hth()
    {
        // sl.append("ERR");
        // clang-format off
        QStringList sl
        {
            "ADCI",
            "FLS",
            "TUP",
            "ADCB",
            "1PPS",
            "ADCM",
            "REGB",
            "RCN",
            "HWIB",
            "HWIM",
            "REGM",
            "BAT",
            "NTP",
            "FLS2",
            "FRM"
        };
        // clang-format on
        return sl;
    }

    static QStringList HthToolTip()
    {
        // sl.append("Что-то не в порядке");
        // clang-format off
        QStringList sl
        {
            "Проблемы со встроенным АЦП ",
            "Не работает внешняя flash-память",
            "Перегрев",
            "Проблемы с АЦП (нет связи) (базовая)",
            "Нет сигнала 1PPS с антенны",
            "Проблемы с АЦП (нет связи) (мезонин)",
            "Ошибка регулировочных коэффициентов (базовая)",
            "Ошибка загрузки конфигурации из flash-памяти. Работает конфигурация по умолчанию",
            "Некорректная Hardware информация (базовая)",
            "Некорректная Hardware информация (мезонин)",
            "Ошибка регулировочных коэффициентов (мезонин)",
            "Напряжение батареи низко (< 2,5 В)",
            "Нет связи с NTP-сервером",
            "Не работает внешняя flash-память (мезонин)",
            "Не работает внешняя fram",
            "Проблемы со встроенным АЦП "
        };
        // clang-format on
        return sl;
    }

    Coma(QWidget *parent = nullptr);
    ~Coma();
    void SetMode(int mode);
    void Go(const QString &parameter = "");
    void ClearTW();
    void SetupMenubar();
    QWidget *MainInfoWidget();

    QWidget *Least();
    // Error::Msg CheckPassword();
    void Disconnect();
    void Connect();

signals:
    void CloseConnectDialog();
    // void PasswordChecked();
    void ClearBsi();
    void Finished();
    void StopCommunications();
    // void ConnectMes(QString *);
    // void stateChanged(bool);

public slots:
    void DisconnectAndClear();

    void FileTimeOut();
    void ReConnect();
    void AttemptToRec();
    void ConnectMessage();

private slots:
    void StartWork();
    void GetAbout();
    void closeEvent(QCloseEvent *event) override;
    void SetDefConf();

    void setConf(unsigned char);
    void Fill();
    void FillBSI(IEC104Thread::BS104Signals *sig);
    void FillBSI(QList<ModBus::BSISignalStruct> sig, unsigned int sigsize);
    // void PasswordCheck(QString psw);
    void SetProgressBar1Size(int size);
    void SetProgressBar1(int cursize);
    void SetProgressBar2Size(int size);
    void SetProgressBar2(int cursize);
    void MainTWTabClicked(int tabindex);

private:
    // constexpr QVector<int> MTBs = { 0x21, 0x22, 0x31, 0x35, 0x80, 0x81, 0x84 };

    AlarmWidget *AlarmW;
    WarnAlarmKIV *WarnAlarmKIVDialog;
    AvarAlarmKIV *AvarAlarmKIVDialog;
    WarnAlarmKTF *WarnAlarmKTFDialog;
    AvarAlarmKTF *AvarAlarmKTFDialog;
    AlarmStateAll *AlarmStateAllDialog;
    AlarmClass *Alarm;
    QWidget *Parent;

    InfoDialog *infoDialog;
    ConfDialog *mainConfDialog;
    ConfDialog *mainTuneDialog;
    AbstractCorDialog *corDialog;
    AbstractConfDialog *confBDialog, *confMDialog;
    EAbstractCheckDialog *checkBDialog, *checkMDialog, *HarmDialog, *VibrDialog;
#ifdef AVM_DEBUG
    EAbstractTuneDialog *tuneDialog;
#endif
    JournalDialog *jourDialog;
    fwupdialog *fwUpDialog;
    MNKTime *timeDialog;

    bool PasswordValid;
    bool TimeThrFinished;
    bool Cancelled;
    bool Reconnect;
    int Mode; // режим запуска программы
    int fileSize, curfileSize;
    int CheckIndex, TimeIndex, ConfIndex, CheckHarmIndex, CheckVibrIndex, CurTabIndex;
    quint8 HaveAlreadyRed = 0;
    quint8 ActiveThreads;
    quint32 Mes;

    QString SavePort;

    QVector<S2::DataRec> *S2ConfigForTune;
    QVector<S2::DataRec> *S2Config;

    quint8 PredAlarmEvents[20];
    quint8 AlarmEvents[20];

    QTimer *ReceiveTimer;
    QTimer *ReconnectTimer;
    QTimer *BdaTimer, *TimeTimer, *AlrmTimer, *HarmTimer, *VibrTimer;

    IEC104 *Ch104;
    ModBus *ChModbus;

    ConnectDialog::ConnectStruct ConnectSettings;

    void LoadSettings();
    void SaveSettings();
    void SetProgressBarSize(int prbnum, int size);
    void SetProgressBar(int prbnum, int cursize);
    void New104();
    void NewModbus();
    void NewUSB();
    void NewTimers();
    void SetupUI();
    void CloseDialogs();
    void PrepareDialogs();
    void NewTimersBda();

    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

    void addConfTab(ETabWidget *MainTW, QString str);
    // Дурацкое название
    void setupQConnections();

    void setupDialogs(ETabWidget *MainTW);

    QToolBar * createToolBar();
    
protected:
    void keyPressEvent(QKeyEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
};

#endif // COMA_H
