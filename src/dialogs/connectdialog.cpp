#include "connectdialog.h"

#include "../module/board.h"
#include "../widgets/wd_func.h"
#include "interfaceemudialog.h"
#include "interfaceethernetdialog.h"
#include "interfaceserialdialog.h"
#include "interfaceusbdialog.h"

#include <QCoreApplication>
#include <QDebug>
#include <QVBoxLayout>
#include <gen/error.h>

ConnectDialog::ConnectDialog(QWidget *parent) : QDialog(parent)
{
    QStringList intersl { "USB" };
    // TODO: использовать AppConfiguration::Service и AppConfiguration::Debug
    if (QCoreApplication::applicationName().contains("service", Qt::CaseInsensitive))
        //        intersl += QStringList { "Ethernet", "RS485" };
        intersl += QStringList { "RS485" };
#ifdef ENABLE_EMULATOR
    intersl.push_back("Emulator");
#endif
    setMinimumWidth(150);
    setAttribute(Qt::WA_DeleteOnClose);
    settings.beginGroup("settings");

    auto layout = new QVBoxLayout;
    layout->addWidget(WDFunc::NewLBL2(this, "Выберите интерфейс связи"));
    auto intercb = WDFunc::NewCB2(this, "intercb", intersl);
    if (settings.contains("LastConnectionType"))
    {
        auto lastConnectionType = settings.value("LastConnectionType").toString();
        intercb->setCurrentIndex(intersl.indexOf(lastConnectionType));
    }
    layout->addWidget(intercb);
    QHBoxLayout *hlyout = new QHBoxLayout;
    auto nextButton = new QPushButton("Далее");
    connect(nextButton, &QPushButton::clicked, this, &ConnectDialog::setInterface);
    hlyout->addWidget(nextButton);
    nextButton = new QPushButton("Отмена");
    connect(nextButton, &QAbstractButton::clicked, this, &QDialog::close);
    hlyout->addWidget(nextButton);
    layout->addLayout(hlyout);
    setLayout(layout);
}

void ConnectDialog::setInterface()
{
    auto &board = Board::GetInstance();
    auto comboBox = this->findChild<QComboBox *>();
    if (comboBox != nullptr)
    {
        auto connectionType = comboBox->currentText();
        // TODO: это не должно тут происходить...
        board.setProperty("interface", connectionType);
        settings.setValue("LastConnectionType", connectionType);
    }

    switch (board.interfaceType())
    {
    case Interface::IfaceType::USB:
        m_idialog = new InterfaceUSBDialog(this);
        break;
    case Interface::IfaceType::Ethernet:
        m_idialog = new InterfaceEthernetDialog(this);
        break;
    case Interface::IfaceType::RS485:
        m_idialog = new InterfaceSerialDialog(this);
        break;
#ifdef ENABLE_EMULATOR
    case Board::InterfaceType::Emulator:
        m_idialog = new InterfaceEmuDialog(this);
        break;
#endif
    default:
        return;
    }
    connect(m_idialog, &AbstractInterfaceDialog::accepted, this, &ConnectDialog::accepted);
    // closing dialogs after selecting device
    connect(m_idialog, &AbstractInterfaceDialog::accepted, this, //
        [this](const ConnectStruct &) {
            m_idialog->close();
            close();
        });

    m_idialog->setupUI();
    if (m_idialog->updateModel())
    {
        m_idialog->adjustSize();
        m_idialog->exec();
    }
    else
        m_idialog->deleteLater();
}
