#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QSplashScreen>
#include "../config/confdialog.h"
#include "../check/eabstractcheckdialog.h"
#ifndef MODULE_A1
#include "../dialogs/oscdialog.h"
#include "../dialogs/switchjournaldialog.h"
#include "eoscillogram.h"
#endif
#include "../tune/eabstracttunedialog.h"
#include "../gen/modulebsi.h"
#include "../gen/maindef.h"
#include "../dialogs/swjdialog.h"
#include "../dialogs/fwupdialog.h"
#include "../dialogs/cordialog.h"
#include "../dialogs/time.h"

#include "eabstractprotocomchannel.h"

#define C_TE_MAXSIZE    100

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

    int Mode; // режим запуска программы
    bool SWHide;
    QRect SWGeometry;
    QVector<S2::DataRec> S2Config;
    QVector<S2::DataRec> S2ConfigForTune;
    ConfDialog *MainConfDialog;
    ConfDialog *MainTuneDialog;
    AbstractConfDialog *ConfB, *ConfM;
    EAbstractCheckDialog *CheckB, *CheckM;
    MNKTime *Time;
#ifndef MODULE_A1
    SwitchJournalDialog *SwjD;
    OscDialog *OscD;
    CorDialog *CorD;
    EOscillogram *OscFunc;
    SWJDialog *dlg;
    fwupdialog *FwUpD;
    void LoadOscFromFile(const QString &filename);
    void LoadSwjFromFile(const QString &filename);
#endif
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
        sl.append("ERR");
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
        int ts = sl.size();
        for (int i=ts; i<MAXERRORFLAGNUM; ++i)
            sl.append("");
        return sl;
    }

    static QStringList HthToolTip()
    {
        QStringList sl;
        sl.append("Что-то не в порядке");
        sl.append("Проблемы с встроенным АЦП ");
        sl.append("Не работает внешняя flash-память");
        sl.append("Перегрев");
        sl.append("Проблемы с АЦП (нет связи) (базовая)");
        sl.append("Нет сигнала 1PPS с антенны");
        sl.append("Проблемы с АЦП (нет связи) (Мезонин)");
        sl.append("Ошибка регулировочных коэффициентов (базовая)");
        sl.append("Ошибка загрузки конфигурации из flash-памяти. Работает конфигурация по умолчанию");
        sl.append("Некорректная Hardware информация (базовая)");
        sl.append("Некорректная Hardware информация (мезонин)");
        sl.append("Ошибка регулировочных коэффициентов (Мезонин)");
        sl.append("Напряжение батареи низко (< 2,5 В)");
        sl.append("Нет связи с NTP-сервером");
        sl.append("Не работает внешняя flash-память (Мезонин)");
        sl.append("Не работает внешняя fram");
        int ts = sl.size();
        for (int i=ts; i<MAXERRORFLAGNUM; ++i)
            sl.append("");
        return sl;
    }

    virtual void SetupUI() = 0;
    virtual void AddActionsToMenuBar(QMenuBar *menubar) = 0;
    virtual void Stage3() = 0;
    void ClearTW();
    void SetupMenubar();
    QWidget *MainInfoWidget();
    QWidget *HthWidget();
    QWidget *Least();
#if PROGSIZE >= PROGSIZE_LARGE
    void SetSlideWidget();
#endif
//    void SetParent(QWidget *parent);

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
    bool TEEnabled; // признак того, ведётся ли лог в правом выезжающем окне
#ifdef USBENABLE
    EAbstractProtocomChannel::DeviceConnectStruct DevInfo;
#endif

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


private slots:
    void StartSettingsDialog();
    void ShowErrorDialog();
    void GetAbout();
    void DisconnectAndClear();
    void FinishHim();
    void closeEvent(QCloseEvent *event);
#ifndef MODULE_A1
    void LoadOsc();
    void LoadSWJ();
    void ShowOsc();
#endif
    void ProtocolFromFile();
    void StartA1Dialog(const QString &filename);

    void SetDefConf();
    void SetMainDefConf();
    void SetBDefConf();
    void SetMDefConf();
    void Fill();
    void PasswordCheck(QString psw);
    void AdminPasswordCheck(QString psw);
#if PROGSIZE != PROGSIZE_EMUL
    void GetDeviceFromTable(QModelIndex idx);
    void Stage1_5();
    void Stage2();
    void UpdateHthWidget();
    void SetProgressBar1Size(int size);
    void SetProgressBar1(int cursize);
    void SetProgressBar2Size(int size);
    void SetProgressBar2(int cursize);
    void ShowConnectDialog();
#endif
#if PROGSIZE >= PROGSIZE_LARGE || PROGSIZE == PROGSIZE_EMUL
    void StartEmul();
#endif
#if PROGSIZE >= PROGSIZE_LARGE
    void UpdateMainTE(QByteArray ba);
    void SetTEEnabled(bool enabled);
    int OpenBhbDialog();
    void MouseMove();

#endif
    void ShowErrorMessageBox(QString message);
    void SetPortSlot(QString port);

protected:
    bool Autonomous; // признак автономного режима

    void keyPressEvent(QKeyEvent *e);
    void resizeEvent(QResizeEvent *e);
};

#endif // MAINWINDOW_H
