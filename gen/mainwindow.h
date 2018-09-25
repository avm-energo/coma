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
    ConfDialog *MainConfDialog;
    AbstractConfDialog *ConfB, *ConfM;
    EAbstractCheckDialog *CheckB, *CheckM;
#ifndef MODULE_A1
    SwitchJournalDialog *SwjD;
    OscDialog *OscD;
    EOscillogram *OscFunc;
    void LoadOscFromFile(const QString &filename);
    void LoadSwjFromFile(const QString &filename);
#endif
    int CheckPassword();

    EAbstractTuneDialog *TuneB, *TuneM;
    void Disconnect();
    QWidget *Parent;
    QSplashScreen *StartWindowSplashScreen;

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
        for (int i=ts; i<MAXERRORFLAGNUM; ++i)
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
    void BsiRefresh();
    void ClearBsi();
    void Finished();

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
#ifndef MODULE_A1
    void LoadOsc();
    void LoadSWJ();
#endif
    void ProtocolFromFile();
    void StartA1Dialog(const QString &filename);

    void SetDefConf();
    void SetMainDefConf();
    void SetBDefConf();
    void SetMDefConf();
    void Fill();
    void PasswordCheck(QString psw);
#if PROGSIZE != PROGSIZE_EMUL
    void GetDeviceFromTable(QModelIndex idx);
    void Stage1_5();
    void Stage2();
    void SetProgressBar1Size(int size);
    void SetProgressBar1(int cursize);
    void SetProgressBar2Size(int size);
    void SetProgressBar2(quint32 cursize);
    void ShowConnectDialog();
#endif
#if PROGSIZE >= PROGSIZE_LARGE || PROGSIZE == PROGSIZE_EMUL
    void StartEmul();
#endif
#if PROGSIZE >= PROGSIZE_LARGE
    void UpdateMainTE(QByteArray ba);
    void SetTEEnabled(bool enabled);
    void OpenBhbDialog();
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
