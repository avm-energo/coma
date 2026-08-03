#include "dialogs/connDialogs/connectdialog.h"

#include <dialogs/connDialogs/emuDialog/interfaceemudialog.h>
#include <dialogs/connDialogs/ethernetDialog/interfaceethernetdialog.h>
#include <dialogs/connDialogs/serialDialog/interfaceserialdialog.h>
#include <dialogs/connDialogs/usbDialog/interfaceusbdialog.h>
#include <libavm-gen/error.h>
#include <libavm-widgets/wdfunc.h>

#include <QAction>
#include <coma.h>

ConnectDialog::ConnectDialog(QWidget *parent) : QMenu(parent), m_idialog(nullptr)
{
    settings.beginGroup("settings");

    QStringList intersl { "USB", "RS485", "Ethernet" };
#ifdef ENABLE_EMULATOR
    intersl.push_back("Emulator");
#endif

    for (const auto &connectionType : intersl)
        connect(addAction(connectionType), &QAction::triggered, this, //
            [this, connectionType] { setInterface(connectionType); });
}

void ConnectDialog::setInterface(const QString &connectionType)
{
    settings.setValue("LastConnectionType", connectionType);

    auto *mainWindow = qobject_cast<Coma *>(WDFunc::getMainWindow());
    if (mainWindow == nullptr)
        return;

    // USB/RS485/Ethernet встраиваются прямо в главное окно вместо отдельного модального окна
    AbstractInterfaceDialog *embedded = nullptr;
    if (connectionType == "USB")
        embedded = new InterfaceUSBDialog(nullptr);
    else if (connectionType == "RS485")
        embedded = new InterfaceSerialDialog(nullptr);
    else if (connectionType == "Ethernet")
        embedded = new InterfaceEthernetDialog(nullptr);

    if (embedded == nullptr)
        return;

    embedded->setupUI();
    if (!embedded->updateModel())
    {
        embedded->deleteLater();
        return;
    }

    connect(embedded, &AbstractInterfaceDialog::accepted, mainWindow, &Coma::initConnection);
    connect(embedded, &AbstractInterfaceDialog::accepted, embedded, //
        [embedded](const ConnectionSettings &) { embedded->close(); });

    // showCentralWidget() сама подписывается на destroyed(), чтобы вернуть index 0
    mainWindow->showCentralWidget(embedded);
    return;

    // Эмулятор не работает?

    // #ifdef ENABLE_EMULATOR
    //     if (connectionType == "Emulator")
    //         m_idialog = new InterfaceEmuDialog(this);
    //     else
    //         m_idialog = nullptr;
    // #endif

    //     if (!m_idialog)
    //         return;

    //     connect(m_idialog, &AbstractInterfaceDialog::accepted, this, &ConnectDialog::accepted);
    //     // closing dialog after selecting device
    //     connect(m_idialog, &AbstractInterfaceDialog::accepted, this, //
    //         [this](const ConnectionSettings &) { m_idialog->close(); });

    //     m_idialog->setupUI();
    //     if (m_idialog->updateModel())
    //     {
    //         m_idialog->adjustSize();
    //         m_idialog->exec();
    //     }
    //     else
    //         m_idialog->deleteLater();
}
