#include "connectdialog.h"

#include "../widgets/wd_func.h"
#include "interfaceemudialog.h"
#include "interfaceethernetdialog.h"
#include "interfaceserialdialog.h"
#include "interfaceusbdialog.h"

#include <QCoreApplication>
#include <QDebug>
#include <QVBoxLayout>
#include <gen/error.h>

ConnectDialog::ConnectDialog(QWidget *parent) : QDialog(parent), m_idialog(nullptr)
{
    QStringList intersl { "USB", "RS485", "Ethernet" };
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
    setWindowTitle("Выбор соединения");
    setLayout(layout);
}

void ConnectDialog::setInterface()
{
    auto comboBox = this->findChild<QComboBox *>();
    if (comboBox)
    {
        auto connectionType = comboBox->currentText();
        settings.setValue("LastConnectionType", connectionType);
        if (connectionType == "USB")
            m_idialog = new InterfaceUSBDialog(this);
        else if (connectionType == "RS485")
            m_idialog = new InterfaceEthernetDialog(this);
        else if (connectionType == "Ethernet")
            m_idialog = new InterfaceSerialDialog(this);
#ifdef ENABLE_EMULATOR
        else if (connectionType == "Emulator")
            m_idialog = new InterfaceEmuDialog(this);
#endif
        else
            m_idialog = nullptr;
    }

    if (!m_idialog)
        return;

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
