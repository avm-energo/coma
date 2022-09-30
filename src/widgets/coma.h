#ifndef COMA_H
#define COMA_H

#include "../dialogs/dialogmanager.h"
#include "../interfaces/settingstypes.h"
#include "../module/module.h"
#include "../oscillograms/oscmanager.h"
#include "../xml/xmleditor/xmleditor.h"

#include <QApplication>
#include <QListWidget>
#include <QMainWindow>
#include <QStackedWidget>
#include <gen/datamanager/typesproxy.h>

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
    Coma(const AppConfiguration &appCfg, QWidget *parent = nullptr);
    ~Coma();

    void go();
    void clearWidgets();
    void setupMenubar();
    QWidget *least();
    void disconnect();
    void setupConnection();
    void loadOsc(QString &filename);
    void loadSwj(QString &filename);

public slots:
    void disconnectAndClear();
    void reconnect();
    void attemptToRec();

private slots:
    void prepareConnectDlg();
    void startWork(const ConnectStruct st);
    void loadOsc();
    void loadSwj();
    void openModuleEditor();
    void getAbout();
    void closeEvent(QCloseEvent *event) override;
    void update(const QVariant &msg);
    void mainTWTabChanged(int tabindex);

private:
    UniquePointer<Module> module;
    QTimer *BdaTimer, *AlrmTimer;
    AlarmWidget *AlarmW;
    UniquePointer<DataTypesProxy> proxyBS, proxyGRS;
    QStackedWidget *MainTW;
    QListWidget *MainLW;
    OscManager oscManager;
    XmlEditor *editor;
    File::Vector fileVector;
    bool Reconnect;
    ConnectStruct ConnectSettings;
    const AppConfiguration &mAppConfig;
    DialogManager *mDlgManager;

    void initInterfaceConnection();
    void loadSettings();
    void saveSettings();
    void setProgressBarSize(int prbnum, int size);
    void setProgressBarCount(int prbnum, int count);
    void newTimers();
    void setupUI();
    void unpackProgramData();
    void setupConnections();
    void prepare();
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
    QToolBar *createToolBar();
    void keyPressEvent(QKeyEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void prepareDialogs();

signals:
    void sendMessage(void *);
};

class ComaHelper
{
public:
    static void initAppSettings(const QString &appName, const QString &orgName, const QString &version);
    static void parserHelper(Coma *coma);
};

#endif // COMA_H
