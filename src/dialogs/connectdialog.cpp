#include "connectdialog.h"

#include "../gen/board.h"
#include "../gen/error.h"
#include "../widgets/wd_func.h"
#include "interfaceethernetdialog.h"
#include "interfaceserialdialog.h"
#include "interfaceusbdialog.h"

#include <QCoreApplication>
#include <QDebug>
#include <QVBoxLayout>

ConnectDialog::ConnectDialog(QWidget *parent) : QDialog(parent)
{
    QStringList intersl;
    if (QCoreApplication::applicationName().contains("service", Qt::CaseInsensitive))
        intersl = QStringList { "USB", "Ethernet", "RS485" };
    if (QCoreApplication::applicationName().contains("debug", Qt::CaseInsensitive))
        intersl = QStringList { "USB" };
    setMinimumWidth(150);
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *lyout = new QVBoxLayout;

    lyout->addWidget(WDFunc::NewLBL2(this, "Выберите интерфейс связи"));
    lyout->addWidget(WDFunc::NewCB2(this, "intercb", intersl));
    QHBoxLayout *hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Далее");
    connect(pb, &QPushButton::clicked, this, &ConnectDialog::setInterface);
    hlyout->addWidget(pb);
    pb = new QPushButton("Отмена");
    connect(pb, &QAbstractButton::clicked, this, &QDialog::close);
    // connect(pb, &QPushButton::clicked, this, &ConnectDialog::Cancelled);
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout);
    setLayout(lyout);
}

void ConnectDialog::setInterface()
{
    auto comboBox = this->findChild<QComboBox *>();
    Board::GetInstance().setProperty("interface", comboBox->currentText());

    switch (Board::GetInstance().interfaceType())
    {
    case Board::InterfaceType::USB:
    {
        m_idialog = new InterfaceUSBDialog(this);
        break;
    }
    case Board::InterfaceType::Ethernet:
    {
        m_idialog = new InterfaceEthernetDialog(this);
        break;
    }
    case Board::InterfaceType::RS485:
    {
        m_idialog = new InterfaceSerialDialog(this);
        break;
    }
    default:
    {
        return;
    }
    }
    connect(m_idialog, &AbstractInterfaceDialog::accepted, this, &ConnectDialog::accepted);
    //      connect(m_idialog, &AbstractInterfaceDialog::accepted, this, &QDialog::close);
    m_idialog->setupUI();
    if (m_idialog->updateModel())
        m_idialog->exec();
    else
        m_idialog->deleteLater();
}
