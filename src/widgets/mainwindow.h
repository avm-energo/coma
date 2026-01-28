#pragma once

#include "../check/eabstractcheckdialog.h"
#include "../config/confdialog.h"
#include "../dialogs/mnktime.h"
#include "../gen/eabstractprotocomchannel.h"
#include "../gen/maindef.h"
#include "../gen/modulebsi.h"
#include "../tune/eabstracttunedialog.h"

#include <QMainWindow>
#include <QModelIndex>
#include <QSplashScreen>

#define C_TE_MAXSIZE 100

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void SetMode(int mode);
    int GetMode();
    void Go();

    enum Modes
    {
        COMA_GENERALMODE,    // обычный режим
        COMA_AUTON_OSCMODE,  // автономный режим с отображением сохранённой осциллограммы
        COMA_AUTON_PROTMODE, // автономный режим с отображением протокола из прибора
        COMA_AUTON_SWJMODE,  // автономный режим с отображением сохранённого журнала
        COMA_AUTON_MODE      // просто автономный режим
    };

    int Mode;                // режим запуска программы
    QVector<S2::DataRec> S2Config;
    QVector<S2::DataRec> S2ConfigForTune;
    ConfDialog *MainConfDialog;
    ConfDialog *MainTuneDialog;
    AbstractConfDialog *ConfB, *ConfM;
    EAbstractCheckDialog *CheckB, *CheckM;
    MNKTime *Time;
    QTimer *TimeTimer;
    int CheckPassword();
    int AdminCheckPassword();

    EAbstractTuneDialog *TuneB, *TuneM;
    void Disconnect();
    QWidget *Parent;
    QSplashScreen *StartWindowSplashScreen;
    QThread *thr;
    int admin;

    static QStringList Hth()
    {
        QStringList sl;
        sl.append("!OK");
        sl.append("FLS");
        sl.append("TUP");
        sl.append("REL");
        sl.append("1PPS");
        sl.append("ADC");
        sl.append("REG");
        sl.append("CNF");
        sl.append("LS");
        sl.append("FNC");
        sl.append("FW");
        sl.append("BAT");
        int ts = sl.size();
        for (int i = ts; i < MAXERRORFLAGNUM; ++i)
            sl.append("");
        return sl;
    }

    static QStringList HthToolTip()
    {
        QStringList sl;
        sl.append("Что-то не в порядке");
        sl.append("Ошибка внешней FLASH-памяти");
        sl.append("Перегрев");
        sl.append("Неисправность выходных реле");
        sl.append("Нет сигнала 1PPS с антенны");
        sl.append("Нет связи с АЦП");
        sl.append("Нет настроечных параметров в памяти");
        sl.append("Ошибка конфигурации в памяти, взята конфигурация по умолчанию");
        sl.append("Сигналы слишком малы");
        sl.append("Частота находится вне диапазона");
        sl.append("Ошибка файла фирменного ПО");
        sl.append("Требуется замена батарейки");
        int ts = sl.size();
        for (int i = ts; i < MAXERRORFLAGNUM; ++i)
            sl.append("");
        return sl;
    }

    virtual void SetupUI() { }
    virtual void AddActionsToMenuBar(QMenuBar *menubar) = 0;
    virtual void Stage3() = 0;
    void ClearTW();
    void SetupMenubar();
    QWidget *MainInfoWidget();
    QWidget *HthWidget();
    QWidget *Least();

signals:
    void CloseConnectDialog();
    void Retry();
    void FinishAll();
    void PasswordChecked();
    void AdminPasswordChecked();
    void BsiRefresh();
    void ClearBsi();
    void Finished();
    void stoptime();

private:
    bool ok;
    EAbstractProtocomChannel::DeviceConnectStruct DevInfo;
    bool m_cancelled;

#if PROGSIZE >= PROGSIZE_LARGE
#endif
    void LoadSettings();
    void SaveSettings();
    void SetProgressBarSize(QString prbnum, int size);
    void SetProgressBar(QString prbnum, int cursize);

private slots:
    void StartSettingsDialog();
    void ShowErrorDialog();
    void GetAbout();
    void DisconnectAndClear();
    void FinishHim();
    void closeEvent(QCloseEvent *event);
    void ProtocolFromFile();
    void StartA1Dialog(const QString &filename);

    void SetDefConf();
    void SetMainDefConf();
    void SetBDefConf();
    void SetMDefConf();
    void Fill();
    void PasswordCheck(QString psw);
    void AdminPasswordCheck(QString psw);
    void GetDeviceFromTable(QModelIndex idx);
    void Stage1_5();
    void Stage2();
    void UpdateHthWidget();
    void SetProgressBar1Size(int size);
    void SetProgressBar1(int cursize);
    void SetProgressBar2Size(int size);
    void SetProgressBar2(int cursize);
    void ShowConnectDialog();
#if PROGSIZE >= PROGSIZE_LARGE
    int OpenBhbDialog();

#endif
    void ShowErrorMessageBox(QString message);
    void SetPortSlot(QString port);

protected:
    bool Autonomous; // признак автономного режима

    void keyPressEvent(QKeyEvent *e);
};
