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

namespace Device
{
class CurrentDevice;
} // namespace Device

namespace Core
{

class Coma : public QMainWindow
{
    Q_OBJECT

public:
    explicit Coma(const AppConfiguration appCfg, QWidget *parent = nullptr);
    virtual ~Coma();

    void go();
    void connectSetupBar();
    void setupMenubar();
    QWidget *least();
    static QPoint ComaCenter();

public slots:
    void disconnectAndClear();
    void loadOsc(const QString &filename);
    void loadSwj(const QString &filename);
    void loadJournal(const QString &filename);

private slots:
    void connectDialog();
    void initConnection(const ConnectStruct &st);
    void initDevice(Interface::AsyncConnection *connection);
    void loadOsc();
    void loadSwj();
    void loadSwjPackConvertor();
    void openJournalViewer();
    void openXmlEditor();
    void showAboutDialog();
    void closeEvent(QCloseEvent *event) override;
    void update(const DataTypes::GeneralResponseStruct &rsp);
    void showReconnectDialog();

private:
    AppConfiguration m_appConfig;
    // Interface::AsyncConnection *m_currentConnection;
    Interface::ConnectionManager *m_connectionManager;
    Device::CurrentDevice *m_currentDevice;
    // UniquePointer<Module> module;
    UniquePointer<S2DataManager> s2dataManager;
    UniquePointer<S2RequestService> s2requestService;
    UniquePointer<DialogManager> m_dlgManager;

    AlarmWidget *AlarmW;
    OscManager oscManager;
    XmlEditor *editor;
    File::Vector fileVector;

    void initInterfaceConnection();
    void loadSettings();
    void saveSettings();
    void setProgressBarSize(int prbnum, int size);
    void setProgressBarCount(int prbnum, int count);
    void setupUI();
    // void prepare();
    QToolBar *createToolBar();
    void keyPressEvent(QKeyEvent *event) override;
    void prepareDialogs();
};

class ComaHelper
{
public:
    static void initAppSettings(const QString &appName, const QString &orgName, const QString &version);
    static void parserHelper(Coma *coma);
};

}
