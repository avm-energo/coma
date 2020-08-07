#ifndef COMA_H
#define COMA_H

#include <QMainWindow>
#include <QResizeEvent>
#include "../config/confdialog.h"
#include "../check/eabstractcheckdialog.h"
#include "../dialogs/cordialog.h"
#include "../dialogs/cordialogktf.h"

#include "../dialogs/infodialog.h"
#include "../dialogs/fwupdialog.h"
#include "../dialogs/mnktime.h"
#include "../dialogs/journalsdialog.h"
#include "../dialogs/abstractcordialog.h"
#include "../alarm/abstractalarm.h"
#include "../alarm/avaralarmkiv.h"
#include "../alarm/alarmwidget.h"
#include "../alarm/modbusupdatepredalarm84.h"
#include "../alarm/modbusupdatestatepredalarm84.h"
#include "../alarm/predalarmkiv.h"
#include "../alarm/updatepredalarm84.h"
#include "../alarm/updatestatepredalarm84.h"
#include "../alarm/usbalarm84.h"
#include "../alarm/usbsetalarm84.h"
#include "../alarm/avaralarmktf.h"
#include "../alarm/predalarmktf.h"
#include "../alarm/alarmstateall.h"


#define RECONNECTINTERVAL   3000
#define WAITINTERVAL        15000

#define THREADUSB   0x01
#define THREAD104   0x02
#define THREADMBS   0x04

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

    const quint32 PredBSIMask = 0x00005F55;
    const quint32 AvarBSIMask = 0x000020AA;

    static QStringList HthToolTip()
    {
        QStringList sl;
        //sl.append("Что-то не в порядке");
        sl.append("Проблемы со встроенным АЦП ");
        sl.append("Не работает внешняя flash-память");
        sl.append("Перегрев");
        sl.append("Проблемы с АЦП (нет связи) (базовая)");
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
        return sl;
    }


     QTimer* BdaTimer;

    Coma(QWidget *parent = nullptr);
    ~Coma();
    void SetMode(int mode);
    void Go(const QString &parameter = "");
    void ClearTW();
    void SetupMenubar();
    QWidget *MainInfoWidget();



    QWidget *ReleWidget();
    QWidget *SignWidget();
    QWidget * WWidgetComa();



    QWidget *Least();
    int CheckPassword();
    void Disconnect();
    void ShowConnectDialog();
    void ShowInterfaceDialog();

signals:
    void CloseConnectDialog();
    void PasswordChecked();
    void ClearBsi();
    void Finished();
    void StopCommunications();
    void ConnectMes(QString*);
   // void SetPredAlarmColor(quint8*);
  //  void SetAlarmColor(quint8*);

public slots:
    void DisconnectAndClear();
   // void UpdatePredAlarmEvents(IEC104Thread::SponSignals *);
   // void UpdateStatePredAlarmEvents(IEC104Thread::SponSignals *);
    void CheckTimeFinish();
    void CheckModBusFinish();
  //  void DeviceState();
  //  void PredAlarmState();
  //  void AlarmState();
    void FileTimeOut();
  //  void UpdateUSB();
  //  void USBSetAlarms();
 //   void ModbusUpdateStatePredAlarmEvents(ModBus::Coils Signal);
  //  void ModBusUpdatePredAlarmEvents(ModBus::Coils Signal);
    void SetCancelled();
    void ReConnect();
    void AttemptToRec();
    void ConnectMessage();

private slots:
    void StartWork();
    void StartSettingsDialog();
    void ShowErrorDialog();
    void GetAbout();
    void closeEvent(QCloseEvent *event);
    void SetDefConf();
    void SetMainDefConf();
    void SetBDefConf();
    void SetMDefConf();
    void Fill();
    void FillBSI(IEC104Thread::BS104Signals *sig);
    void FillBSI(QList<ModBus::BSISignalStruct> sig, unsigned int sigsize);
    void PasswordCheck(QString psw);
    void SetPortSlot(QString port);
    void SetProgressBar1Size(int size);
    void SetProgressBar1(int cursize);
    void SetProgressBar2Size(int size);
    void SetProgressBar2(int cursize);
    void ShowErrorMessageBox(QString message);
    void ParseString(QString Str);
    void ParseInter(QString str);
    void MainTWTabClicked(int tabindex);

    // finished slots
    void ModBusFinished();
    void Ch104Finished();
    void USBFinished();

private:
    const QVector<int> MTBs = {0x21, 0x22, 0x31, 0x35, 0x80, 0x81, 0x84};

    AbstractCorDialog *CorD;
    AlarmWidget *ALARMW;
    AlarmClass *Alarm;

    InfoDialog *IDialog;
    bool Ok;
    bool Disconnected;
    int Mode; // режим запуска программы
    QVector<S2::DataRec> S2ConfigForTune;
    ConfDialog *MainConfDialog;
    ConfDialog *MainTuneDialog;
    quint8 HaveAlreadyRed = 0;
    quint32 Mes;
    bool TimeThrFinished;
    QTimer *ReceiveTimer;
    quint8 PredAlarmEvents[20];
    quint8 AlarmEvents[20];
    int fileSize, curfileSize;
    QTimer *ReconnectTimer;
    QString SavePort;
    DeviceConnectStruct DevInfo;
    quint8 ActiveThreads;
    int CheckIndex, TimeIndex, ConfIndex, CurTabIndex;
    AbstractConfDialog *ConfB, *ConfM;
    EAbstractCheckDialog *CheckB, *CheckM;
    PredAlarmKIV *PredAlarmKIVWidget;
    AvarAlarmKIV *AvarAlarmKIVWidget;
    PredAlarmKTF *PredAlarmKTFWidget;
    AvarAlarmKTF *AvarAlarmKTFWidget;
    AlarmStateAll *AlarmStateAllWidget;
    AbstractAlarm *AbstrALARM;
    IEC104 *Ch104;
    ModBus *ChModbus;
    MNKTime *TimeD;
    JournalDialog *JourD;
    fwupdialog *FwUpD;
    QString IPtemp, FullName, SaveDevice, instr;
    QStringList sl, USBsl, slfinal;
    quint16 AdrBaseStation;
    SerialPort::Settings Settings;
    QTimer *TimeTimer, *AlarmStateTimer;
    QVector<S2::DataRec> *S2Config;
    QWidget *Parent;
    QWidget *Wpred;
    QWidget *Walarm;
    bool Cancelled;
    bool Reconnect;

    void LoadSettings();
    void SaveSettings();
    void SetProgressBarSize(QString prbnum, int size);
    void SetProgressBar(QString prbnum, int cursize);
    void New104();
    void NewModbus();
    void NewUSB();
    void NewTimers();
    void SetupUI();
    void PrepareDialogs();
    void CloseDialogs();
    void NewTimersBda();

protected:
    void keyPressEvent(QKeyEvent *e);
    void resizeEvent(QResizeEvent *e);
};

#endif // COMA_H
