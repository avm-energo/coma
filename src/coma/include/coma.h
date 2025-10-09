#pragma once

#include <dialogs/dialogmanager.h>
#include <common/comahelper.h>
#include <interfaces/connectionmanager.h>
#include <interfaces/types/connection_settings.h>
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
    static QPoint comaCenter();

public slots:
    void disconnectAndClear();
    void loadSwj(const QString &filename);
    void loadJournal(const QString &filename);

private slots:
    void createSlice();
    void restoreFromSlice();
    void openSlice();
    void connectDialog();
    void initConnection(const ConnectionSettings &st);
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
    QMetaObject::Connection m_prbConnection;
    EStatusBar *m_bar;

    AlarmWidget *m_alarmW;
    OscManager m_oscManager;
    XmlEditor *m_xmlEditor;
    File::Vector m_fileVector;

    void initInterfaceConnection();
    void setProgressBarSize(int prbnum, int size);
    void setProgressBarCount(int prbnum, int count);
    void setupUI();
    QToolBar *createToolBar();
    void keyPressEvent(QKeyEvent *event) override;
    void loadXML();
    void prepareDialogs();

signals:
    void settingsChanged(const QString &name, const QVariant &value);
};
