#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "publicclass.h"
#include "../config/confdialog.h"

#define C_TE_MAXSIZE    100

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = 0);

    bool SWHide;
    QRect SWGeometry;
    QVector<publicclass::DataRec> S2Config;
    AbstractConfDialog *ConfB, *ConfM;
    QWidget *Parent;

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

    void PrepareTimers();
    void LoadSettings();
    void SaveSettings();
#if PROGSIZE >= PROGSIZE_LARGE
    void ShowOrHideSlideSW();
#endif
    int CheckPassword();
    void SetProgressBarSize(QString prbnum, quint32 size);
    void SetProgressBar(QString prbnum, quint32 cursize);


private slots:
    void ShowConnectDialog();
    void Stage1_5();
    void Stage2();
    void SetDefConf();
    void Fill();
    void PasswordCheck(QString &psw);
    void StartSettingsDialog();
    void ShowErrorDialog();
    void SetProgressBar1Size(quint32 size);
    void SetProgressBar1(quint32 cursize);
    void SetProgressBar2Size(quint32 size);
    void SetProgressBar2(quint32 cursize);
    void GetAbout();
    void Disconnect();
    void DisconnectAndClear();
#if PROGSIZE >= PROGSIZE_LARGE
    void UpdateMainTE(QByteArray &ba);
    void OpenBhbDialog();
    void StartEmul();
    void MouseMove();
#endif
    void ShowErrorMessageBox(QString message);
    void SetPortSlot(QString port);

protected:
    void keyPressEvent(QKeyEvent *e);
    void resizeEvent(QResizeEvent *e);
};

#endif // MAINWINDOW_H
