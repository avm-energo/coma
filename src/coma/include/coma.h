#pragma once

#include <dialogs/dialogmanager.h>
#include <helpers/comahelper.h>
#include <interfaces/connectionmanager.h>
#include <interfaces/types/settingstypes.h>
#include <oscillograms/oscmanager.h>
#include <widgets/estatusbar.h>

#include <QApplication>
#include <QListWidget>
#include <QMainWindow>
#include <QStackedWidget>

class AlarmWidget;
class Module;
class XmlEditor;
class ReconnectDialog;

namespace Device
{
class CurrentDevice;
} // namespace Device

class Coma : public QMainWindow
{
    Q_OBJECT

public:
    explicit Coma(QWidget *parent = nullptr);
    virtual ~Coma();

    void go();
    void connectStatusBar();
    void setupMenubar();
    QWidget *least();
    static QPoint ComaCenter();
    ComaHelper *Helper();

public slots:
    void disconnectAndClear();
    void loadSwj(const QString &filename);
    void loadJournal(const QString &filename);

private slots:
    void createSlice();
    void restoreFromSlice();
    void openSlice();
    void connectDialog();
    void initConnection(const ConnectStruct &st);
    void initDevice(Interface::AsyncConnection *connection);
    void loadOsc();
    void loadSwj();
    void loadSwjPackConvertor();
    void hex2BinConverter();
    void openJournalViewer();
    void openXmlEditor();
    void showAboutDialog();
    void closeEvent(QCloseEvent *event) override;
    void update(const DataTypes::GeneralResponseStruct &rsp);
    void showReconnectDialog();

private:
    Interface::ConnectionManager *m_connectionManager;
    Device::CurrentDevice *m_currentDevice;
    UniquePointer<DialogManager> m_dlgManager;
    ReconnectDialog *m_reconnectDialog;
    EStatusBar *m_bar;

    AlarmWidget *AlarmW;
    OscManager oscManager;
    XmlEditor *editor;
    File::Vector fileVector;
    ComaHelper *m_helper;

    void initInterfaceConnection();
    void setProgressBarSize(int prbnum, int size);
    void setProgressBarCount(int prbnum, int count);
    void setupUI();
    QToolBar *createToolBar();
    void keyPressEvent(QKeyEvent *event) override;
    void prepareDialogs();
};
