#ifndef COMA_H
#define COMA_H

#include "../dialogs/dialogmanager.h"
#include "../interfaces/settingstypes.h"
#include "../module/module.h"
#include "../module/s2requestservice.h"
#include "../oscillograms/oscmanager.h"
#include "../s2/s2datamanager.h"
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
    virtual ~Coma();

    void go();
    void connectSB();
    void setupMenubar();
    QWidget *least();
    void disconnect();
    void setupConnection();
    static QPoint ComaCenter();

public slots:
    void disconnectAndClear();
    void reconnect();
    void attemptToRec();
    void loadOsc(const QString &filename);
    void loadSwj(const QString &filename);
    void loadJournal(const QString &filename);

private slots:
    void prepareConnectDlg();
    void startWork(const ConnectStruct st);
    void loadOsc();
    void loadSwj();
    void openJournalViewer();
    void openXmlEditor();
    void showAboutDialog();
    void closeEvent(QCloseEvent *event) override;
    void update(const QVariant &msg);

private:
    UniquePointer<Module> module;
    UniquePointer<S2DataManager> s2dataManager;
    UniquePointer<S2RequestService> s2requestService;

    QTimer *BdaTimer, *AlrmTimer;
    AlarmWidget *AlarmW;
    UniquePointer<DataTypesProxy> proxyBS, proxyGRS;
    OscManager oscManager;
    XmlEditor *editor;
    File::Vector fileVector;
    bool Reconnect;
    ConnectStruct ConnectSettings;
    AppConfiguration mAppConfig;
    UniquePointer<DialogManager> mDlgManager;
    static QPoint s_comaCenter;

    void initInterfaceConnection();
    void loadSettings();
    void saveSettings();
    void setProgressBarSize(int prbnum, int size);
    void setProgressBarCount(int prbnum, int count);
    void newTimers();
    void setupUI();
    void prepare();
    QToolBar *createToolBar();
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void prepareDialogs();
    bool nativeEventHandler(const QByteArray &eventType, void *message);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    virtual bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override
#else
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override
#endif
    {
        Q_UNUSED(result);
        return nativeEventHandler(eventType, message);
    }

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
