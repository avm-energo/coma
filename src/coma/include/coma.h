#pragma once

#include "journals/basejournal.h"
#include <libavm-widgets/estatusbar.h>
#include <common/comahelper.h>
#include <dialogs/dialogmanager.h>
#include <interfaces/connectionmanager.h>
#include <interfaces/types/connection_settings.h>
#include <oscillograms/oscmanager.h>

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
    /// \brief Hides the main window content and shows \a widget in its place.
    void showCentralWidget(QWidget *widget);
    /// \brief Returns from the embedded widget back to the main window content.
    void hideCentralWidget();

public slots:
    void disconnectAndClear();
    void loadSwj(const QString &filename);
    void loadJournal(const QString &filename, journals::JournalType type);
    void initConnection(const ConnectionSettings &st);

private slots:
    void createSlice();
    void restoreFromSlice();
    void openSlice();
    void connectDialog();
    void initDevice(Interface::AsyncConnection *connection);
    void loadOsc();
    void loadSwj();
    void loadSwjPackConvertor();
    void hex2BinConverter();
    void openJournalViewerJN();
    void openJournalViewerDAT();
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
    QStackedWidget *m_centralStack;
    /// \brief destroyed() -> hideCentralWidget() connections made by showCentralWidget(), one per
    /// widget it has ever embedded. Explicitly disconnected in ~Coma() before children are torn
    /// down, since by then Coma itself is no longer safely callable as a slot receiver.
    QList<QMetaObject::Connection> m_centralWidgetConnections;

    void initInterfaceConnection();
    void setProgressBarSize(int prbnum, int size);
    void setProgressBarCount(int prbnum, int count);
    void setupUI();
    QToolBar *createToolBar();
    void keyPressEvent(QKeyEvent *event) override;
    void loadXML();
    void prepareDialogs();
    /// \brief Removes and deletes any widgets embedded into the central stack.
    void clearCentralWidgets();

signals:
    void settingsChanged(const QString &name, const QVariant &value);
};