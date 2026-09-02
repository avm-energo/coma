#include "dialogs/connDialogs/connectdialog.h"

#include <dialogs/connDialogs/emuDialog/interfaceemudialog.h>
#include <dialogs/connDialogs/ethernetDialog/interfaceethernetdialog.h>
#include <dialogs/connDialogs/serialDialog/interfaceserialdialog.h>
#include <dialogs/connDialogs/usbDialog/interfaceusbdialog.h>
#include <libavm-gen/error.h>
#include <libavm-widgets/wdfunc.h>

#include <QPointer>
#include <QPushButton>
#include <QVBoxLayout>
#include <coma.h>

ConnectDialog::ConnectDialog(QWidget *parent) : QWidget(parent, Qt::Popup), m_idialog(nullptr)
{
    settings.beginGroup("settings");

    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("ConnectDialog { background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                  " stop:0 #8A2BE2, stop:1 #000080); }");

    QStringList intersl { "USB", "RS485", "Ethernet" };

#ifdef ENABLE_EMULATOR
    intersl.push_back("Emulator");
#endif

    auto *lyout = new QVBoxLayout(this);
    lyout->setContentsMargins(3, 3, 3, 3);
    lyout->setSpacing(0);

    for (const auto &connectionType : intersl)
    {
        auto *button = new QPushButton(connectionType, this);
        button->setStyleSheet(
            "border-radius: 0px; padding-top: 5px; padding-bottom: 5px; padding-left: 3px; padding-right: 3px;");
        connect(button, &QPushButton::clicked, this, //
            [this, connectionType] { setInterface(connectionType); });
        lyout->addWidget(button);
    }
}

void ConnectDialog::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    // Popup закрывается кликом снаружи/Escape (через hide(), а не close()),
    // поэтому удаляем себя здесь, а не полагаемся на Qt::WA_DeleteOnClose
    deleteLater();
}

void ConnectDialog::setInterface(const QString &connectionType)
{
    settings.setValue("LastConnectionType", connectionType);
    hide();

    auto *mainWindow = qobject_cast<Coma *>(WDFunc::getMainWindow());
    if (mainWindow == nullptr)
        return;

    // USB/RS485/Ethernet встраиваются прямо в главное окно вместо отдельного модального окна
    AbstractInterfaceDialog *ifaceDialog = nullptr;
    if (connectionType == "USB")
        ifaceDialog = new InterfaceUSBDialog(nullptr);
    else if (connectionType == "RS485")
        ifaceDialog = new InterfaceSerialDialog(nullptr);
    else if (connectionType == "Ethernet")
        ifaceDialog = new InterfaceEthernetDialog(nullptr);

    if (ifaceDialog == nullptr)
        return;

    ifaceDialog->setupUI();
    if (!ifaceDialog->updateModel())
    {
        ifaceDialog->deleteLater();
        return;
    }

    connect(ifaceDialog, &AbstractInterfaceDialog::accepted, mainWindow, &Coma::initConnection);
    connect(ifaceDialog, &AbstractInterfaceDialog::accepted, ifaceDialog, //
        [dialog = QPointer<AbstractInterfaceDialog>(ifaceDialog)](const ConnectionSettings &)
        {
            if (dialog)
                dialog->close();
        });

    // showCentralWidget() сама подписывается на destroyed(), чтобы вернуть index 0
    mainWindow->showCentralWidget(ifaceDialog);
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
