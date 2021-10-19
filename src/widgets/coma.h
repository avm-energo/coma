#ifndef COMA_H
#define COMA_H
#include "../avtuk/oscmanager.h"
#include "../gen/datatypes.h"
#include "../gen/stdfunc.h"
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

    Coma(QWidget *parent = nullptr);
    ~Coma();

    void go();
    void clearWidgets();
    void setupMenubar();

    QWidget *least();

    void disconnect();
    virtual void setupConnection();

    void loadOsc(QString &filename);
    void loadSwj(QString &filename);
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

    virtual void getAbout() {};
    void closeEvent(QCloseEvent *event) override;

    void update(const DataTypes::GeneralResponseStruct &rsp);

    void mainTWTabChanged(int tabindex);

private:
    QStackedWidget *MainTW;
    QListWidget *MainLW;
    OscManager oscManager;

    File::Vector fileVector;

    bool Reconnect;

    ConnectStruct ConnectSettings;

    void LoadSettings();
    void SaveSettings();
    void SetProgressBarSize(int prbnum, int size);
    void SetProgressBarCount(int prbnum, int count);
    void newTimers();
    void SetupUI();
    // void CloseDialogs();

    void setupConnections();
    void prepare();

    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

    QToolBar *createToolBar();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    virtual void PrepareDialogs();
signals:
    void sendMessage(void *);
};

class ComaHelper
{
public:
    static void initResources()
    {
        Q_INIT_RESOURCE(darkstyle);
        Q_INIT_RESOURCE(lightstyle);
        Q_INIT_RESOURCE(styles);
        Q_INIT_RESOURCE(vectorIcons);
    }
    static void parserHelper(const char *appName, Coma *coma);
};

#endif // COMA_H
