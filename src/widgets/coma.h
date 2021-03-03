#ifndef COMA_H
#define COMA_H

#include "../gen/datatypes.h"
#include "../gen/stdfunc.h"
#include "../avtuk/oscmanager.h"
#include "../interfaces/settingstypes.h"

#include <QListWidget>
#include <QMainWindow>
#include <QStackedWidget>

class Module;
class AlarmWidget;

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
    using ModulePointer = UniquePointer<Module>;
    enum Modes
    {
        COMA_GENERALMODE, // обычный режим
        COMA_AUTON_OSCMODE, // автономный режим с отображением сохранённой осциллограммы
        COMA_AUTON_PROTMODE, // автономный режим с отображением протокола из прибора
        COMA_AUTON_SWJMODE, // автономный режим с отображением сохранённого журнала
        COMA_AUTON_MODE // просто автономный режим
    };

    Coma(QWidget *parent = nullptr);
    ~Coma();
    void setMode(int mode);
    void go(const QString &parameter = "");
    void clearWidgets();
    void setupMenubar();

    QWidget *least();

    void disconnect();
    virtual void setupConnection();

public slots:
    void DisconnectAndClear();

    void reconnect();
    void attemptToRec();

protected:
    ModulePointer m_Module;
    QTimer *BdaTimer, *AlrmTimer;
    AlarmWidget *AlarmW;
private slots:
    void prepareConnectDlg();
    void startWork(const ConnectStruct st);
    void loadOsc();
    void loadSwj();

    void getAbout();
    void closeEvent(QCloseEvent *event) override;

    void update(const DataTypes::GeneralResponseStruct &rsp);

    void mainTWTabChanged(int tabindex);

private:
    QStackedWidget *MainTW;
    QListWidget *MainLW;
    OscManager *oscManager;

    bool TimeThrFinished;

    bool Reconnect;
    int Mode; // режим запуска программы

    quint8 HaveAlreadyRed = 0;

    quint32 Mes;

    QString SavePort;

    ConnectStruct ConnectSettings;

    void LoadSettings();
    void SaveSettings();
    void SetProgressBarSize(int prbnum, int size);
    void SetProgressBarCount(int prbnum, int count);
    void newTimers();
    void SetupUI();
    // void CloseDialogs();
    virtual void PrepareDialogs() = 0;
    void setupConnections();
    void prepare();

    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

    QToolBar *createToolBar();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
signals:
    void sendMessage(void *);
};

#endif // COMA_H
