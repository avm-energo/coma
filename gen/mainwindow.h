#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QSplashScreen>
#include "../config/confdialog.h"
#include "../gen/modulebsi.h"
#include "../gen/maindef.h"
#include "eabstractprotocomchannel.h"
#include "../iec104/ethernet.h"
#include "../iec104/iec104.h"
#include "../check/eabstractcheckdialog.h"
#include "../check/checkdialog84.h"
#include "../dialogs/journalsdialog.h"
#include "../modbus/modbus.h"
#include "../dialogs/time.h"
#include "../dialogs/fwupdialog.h"


#define C_TE_MAXSIZE    100

#define RECONNECTINTERVAL   3000
#define WAITINTERVAL        1500

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void SetMode(int mode);
    int GetMode();
    void Go(const QString &parameter="");

    enum Modes
    {
        COMA_GENERALMODE, // обычный режим
        COMA_AUTON_OSCMODE, // автономный режим с отображением сохранённой осциллограммы
        COMA_AUTON_PROTMODE, // автономный режим с отображением протокола из прибора
        COMA_AUTON_SWJMODE, // автономный режим с отображением сохранённого журнала
        COMA_AUTON_MODE // просто автономный режим
    };


    static quint32 MTypeB;
    static quint32 MTypeM;
    static int TheEnd, StopRead;


    struct DeviceConnectStruct
    {
        unsigned short vendor_id;
        unsigned short product_id;
        wchar_t serial[20];
    };

    typedef struct
    {
        quint32 SigVal;
        quint8 SigQuality;
        //quint64 CP56Time;
    }BS104;

    struct Bd11
    {
        quint32 dev;
        quint32 predAlarm;
        quint32 alarm;
    };

    Bd11 Bd_block11;

    AbstractConfDialog *ConfB, *ConfM;
    EAbstractCheckDialog *CheckB, *CheckM;
    IEC104 *Ch104;
    ModBus *ChModbus;
    MNKTime *Time;
    JournalDialog *JourD;
    fwupdialog *FwUpD;
    QString IPtemp, FullName, SaveDevice, instr;
    QStringList sl, USBsl, slfinal, insl;
    quint16 AdrBaseStation;
    SerialPort::Settings Settings;
    QTimer* BdaTimer, *TimeTimer, *AlarmStateTimer;
    QVector<S2::DataRec> S2Config;

    int CheckPassword();


    void Disconnect();
    QWidget *Parent;
    QSplashScreen *StartWindowSplashScreen;

    static QStringList Hth()
    {
        QStringList sl;
        //sl.append("ERR");
        sl.append("ADCI");
        sl.append("FLS");
        sl.append("TUP");
        sl.append("ADCB");
        sl.append("1PPS");
        sl.append("ADCM");
        sl.append("REGB");
        sl.append("RCN");
        sl.append("HWIB");
        sl.append("HWIM");
        sl.append("REGM");
        sl.append("BAT");
        sl.append("NTP");
        sl.append("FLS2");
        sl.append("FRM");
        //int ts = sl.size();
        //for (int i=ts; i<MAXERRORFLAGNUM; ++i)
        //    sl.append("");
        return sl;
    }

    static QStringList HthToolTip()
    {
        QStringList sl;
        //sl.append("Что-то не в порядке");
        sl.append("Проблемы со встроенным АЦП ");
        sl.append("Не работает внешняя flash-память");
        sl.append("Перегрев");
        sl.append("Нет сигнала 1PPS с антенны");
        sl.append("Проблемы с АЦП (нет связи) (мезонин)");
        sl.append("Ошибка регулировочных коэффициентов (базовая)");
        sl.append("Ошибка загрузки конфигурации из flash-памяти. Работает конфигурация по умолчанию");
        sl.append("Некорректная Hardware информация (базовая)");
        sl.append("Некорректная Hardware информация (мезонин)");
        sl.append("Ошибка регулировочных коэффициентов (мезонин)");
        sl.append("Напряжение батареи низко (< 2,5 В)");
        sl.append("Нет связи с NTP-сервером");
        sl.append("Не работает внешняя flash-память (мезонин)");
        sl.append("Не работает внешняя fram");
        //int ts = sl.size();
        //for (int i=ts; i<MAXERRORFLAGNUM; ++i)
        //    sl.append("");
        return sl;
    }

    virtual void SetupUI() = 0;
    virtual void AddActionsToMenuBar(QMenuBar *menubar) = 0;
    virtual void Stage3() = 0;
    void ClearTW();
    void SetupMenubar();
    QWidget *MainInfoWidget();
    QWidget *HthWidget();
    QWidget *ReleWidget();
    QWidget *Least();
    QWidget *Wpred;
    QWidget *Walarm;
    bool cancel;
        bool Reconnect;

#if PROGSIZE >= PROGSIZE_LARGE
    void SetSlideWidget();
#endif
//    void SetParent(QWidget *parent);

signals:
    void CloseConnectDialog();
    void Retry();
    void FinishAll();
    void PasswordChecked();
    void BsiRefresh(ModuleBSI::Bsi*);
    void USBBsiRefresh();
    void ClearBsi();
    void Finished();
    void StopCommunications();
//    void stopit();
    void stoptime();
//    void stopModBus();
    void ConnectMes(QString*);
    void SetPredAlarmColor(quint8*);
    void SetAlarmColor(quint8*);

private:
    bool Ok;
    bool TEEnabled; // признак того, ведётся ли лог в правом выезжающем окне
    int Disconnected;
    int Mode; // режим запуска программы
    bool SWHide;
    QRect SWGeometry;
    QVector<S2::DataRec> S2ConfigForTune;
    ConfDialog *MainConfDialog;
    ConfDialog *MainTuneDialog;

    //CheckDialog84* Ch84;
    quint8 HaveAlreadyRed = 0;
    //MNKTime *Time;
    //QThread *thr;
    quint32 Mes;
    bool TimeThrFinished, ModBusThrFinished;
    QTimer *ReceiveTimer;
    quint8 PredAlarmEvents[20];
    quint8 AlarmEvents[20];
    int fileSize, curfileSize;
    QTimer *ReconnectTimer;
    QString SavePort;

    DeviceConnectStruct DevInfo;

#if PROGSIZE >= PROGSIZE_LARGE
    void PrepareTimers();
    void ShowOrHideSlideSW();
#endif
    void LoadSettings();
    void SaveSettings();
#if PROGSIZE != PROGSIZE_EMUL
    void SetProgressBarSize(QString prbnum, int size);
    void SetProgressBar(QString prbnum, int cursize);
#endif

public slots:
    void DisconnectAndClear();
    void FinishHim();
    void UpdateReleWidget(Parse104::SponSignals104*);
    void UpdatePredAlarmEvents(Parse104::SponSignalsWithTime *);
    void UpdateStatePredAlarmEvents(Parse104::SponSignals104*);
    void UpdateStatePredAlarmEventsWithTime(Parse104::SponSignalsWithTime*);
    void CheckTimeFinish();
    void CheckModBusFinish();
    void Stop_BdaTimer(int index);
    void Start_BdaTimer(int index);
    void Stop_TimeTimer(int index);
    void Start_TimeTimer(int index);
    void DeviceState();
    void PredAlarmState();
    void AlarmState();
    void FileTimeOut();
    void GetUSBAlarmTimerTimeout();
    void GetUSBAlarmInDialog();
    void ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal);
    void ModBusUpdatePredAlarmEvents(ModBus::Coils Signal);
    void SetCancelled();
    void ReConnect();
    void attemptToRec();
    void ConnectMessage(QString*);
    void ConnectMessage();


private slots:
    void StartSettingsDialog();
    void ShowErrorDialog();
    void GetAbout();
    void closeEvent(QCloseEvent *event);
    void SetDefConf();
    void SetMainDefConf();
    void SetBDefConf();
    void SetMDefConf();
    void Fill();
    void PasswordCheck(QString psw);
    void DisconnectMessage();
    void SetPortSlot(QString port);


#if PROGSIZE != PROGSIZE_EMUL
    void GetDeviceFromTable(QModelIndex idx);
    void Stage1_5();
    void Stage2();
    void UpdateHthWidget(ModuleBSI::Bsi *);
    void SetProgressBar1Size(int size);
    void SetProgressBar1(int cursize);
    void SetProgressBar2Size(int size);
    void SetProgressBar2(int cursize);
    void ShowConnectDialog();
    void ShowInterfaceDialog();
#endif
#if PROGSIZE >= PROGSIZE_LARGE || PROGSIZE == PROGSIZE_EMUL
    void StartEmul();
#endif
#if PROGSIZE >= PROGSIZE_LARGE
    void UpdateMainTE(QByteArray ba);
    void SetTEEnabled(bool enabled);
    //int OpenBhbDialog();
    void MouseMove();

#endif
    void ShowErrorMessageBox(QString message);
    void ParseString(QString Str);
    void ParseInter(QString str);
    //void SaveModBusString(QString ModBus);
    //void SaveInterface(QString Interface);
    //void SDevice(QString Device);

protected:
    bool Autonomous; // признак автономного режима
    void keyPressEvent(QKeyEvent *e);
    void resizeEvent(QResizeEvent *e);
};

#endif // MAINWINDOW_H

