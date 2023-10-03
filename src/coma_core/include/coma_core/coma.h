#pragma once

#include "../../dialogs/dialogmanager.h"
#include "../../oscillograms/oscmanager.h"

#include <QApplication>
#include <QListWidget>
#include <QMainWindow>
#include <QStackedWidget>
#include <interfaces/connectionmanager.h>
#include <interfaces/types/settingstypes.h>
#include <s2/s2datamanager.h>

class AlarmWidget;
class Module;
class S2RequestService;
class XmlEditor;

namespace Core
{

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
    // void setupConnection();
    static QPoint ComaCenter();

public slots:
    void disconnectAndClear();
    void loadOsc(const QString &filename);
    void loadSwj(const QString &filename);
    void loadJournal(const QString &filename);

private slots:
    void connectDialog();
    void initConnection(const ConnectStruct &st);
    void loadOsc();
    void loadSwj();
    void loadSwjPackConvertor();
    void openJournalViewer();
    void openXmlEditor();
    void showAboutDialog();
    void closeEvent(QCloseEvent *event) override;
    void update(const DataTypes::GeneralResponseStruct &rsp);
    // void nativeEvent(void *message);

private:
    UniquePointer<Interface::ConnectionManager> connectionManager;
    UniquePointer<Module> module;
    UniquePointer<S2DataManager> s2dataManager;
    UniquePointer<S2RequestService> s2requestService;

    QTimer *BdaTimer;
    AlarmWidget *AlarmW;
    OscManager oscManager;
    XmlEditor *editor;
    File::Vector fileVector;
    bool Reconnect;
    AppConfiguration mAppConfig;
    UniquePointer<DialogManager> mDlgManager;

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

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    virtual bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override
#else
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override
#endif
    {
        Q_UNUSED(result);
        if (connectionManager)
            connectionManager->nativeEventHandler(eventType, message);
        return false;
    }

signals:
    // void sendMessage(void *);
    void positionChanged(const QPoint &center);
};

class ComaHelper
{
public:
    static void initAppSettings(const QString &appName, const QString &orgName, const QString &version);
    static void parserHelper(Coma *coma);
};

}
