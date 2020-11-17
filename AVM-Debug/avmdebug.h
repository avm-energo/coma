#ifndef AVMDEBUG_H
#define AVMDEBUG_H

#include "../dialogs/connectdialog.h"
#include "../interfaces/settingstypes.h"
#include "../module/module.h"
#include "../widgets/alarmstateallwidget.h"
#include "../widgets/alarmwidget.h"
#include "../widgets/etabwidget.h"

#include <QMainWindow>
#include <QResizeEvent>

#ifdef _WIN32
// clang-format off
#include <windows.h>
// Header dbt must be the last header, thanx to microsoft
#include <dbt.h>
// clang-format on
#endif

enum INTERVAL
{
    RECONNECT = 3000,
    WAIT = 300000
};

class AvmDebug : public QMainWindow
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
    friend void registerForDeviceNotification(AvmDebug *);

    AvmDebug(QWidget *parent = nullptr);
    ~AvmDebug();
    void SetMode(int mode);
    void Go(const QString &parameter = "");
    void ClearTW();
    void SetupMenubar();
    //    QWidget *MainInfoWidget();

    QWidget *Least();
    //    Error::Msg CheckPassword();
    void Disconnect();
    void Connect();

signals:
    void CloseConnectDialog();
    //    void PasswordChecked();
    void ClearBsi();
    void Finished();
    void StopCommunications();
    //    void ConnectMes(QString *);
    //    void stateChanged(bool);

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
    //    void SetDefConf();
    void update(DataTypes::GeneralResponseStruct &rsp);

    void SetProgressBarSize(int prbnum, int size);
    void SetProgressBar(int prbnum, int cursize);
    void MainTWTabClicked(int tabindex);

private:
    AlarmWidget *AlarmW;
    //    AlarmStateAll *AlarmStateAllDialog;
    //    AlarmClass *Alarm;
    Module *m_Module;
    bool TimeThrFinished;
    bool Cancelled;
    bool Reconnect;
    int Mode; // режим запуска программы
    int fileSize, curfileSize;
    quint8 HaveAlreadyRed = 0;
    bool ActiveUSBThread;
    quint32 Mes;

    QString SavePort;

    S2ConfigType *S2Config;

    QTimer *ReceiveTimer, *m_BSITimer;
    QTimer *BdaTimer, *AlrmTimer;

    ConnectStruct ConnectSettings;

    void LoadSettings();
    void SaveSettings();
    void SetProgressBar1Size(int size);
    void SetProgressBar1(int cursize);
    //    void SetProgressBarSize(int prbnum, int size);
    //    void SetProgressBar(int prbnum, int cursize);
    void NewUSB();
    void NewTimers();
    void SetupUI();
    void CloseDialogs();
    void PrepareDialogs();
    void NewTimersBda();

    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

    //    void setupConnections();
    QToolBar *createToolBar();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
};

#endif // AVMDEBUG_H
