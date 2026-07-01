#include "moduleWorker/moduleWorker.h"
#include "avm-widgets/emessagebox.h"
#include "interfaces/ifaces/usbhidport.h"

#include <QVBoxLayout>

ModuleWorker::ModuleWorker(bool isDry, UsbHidSettings *usb, QWidget *parent)
    : m_isDry(isDry)
    , m_usb(new UsbHidPort(usb, this))
    , QWidget(parent)
{
    createCloseButton();
    setupUI();

    if (!connectToUSB())
    {
        EMessageBox::error(this, "Ошибка подключения устройства");
        return;
    }

    // Нужна ли обработка сигнала BaseInterface::reconnected?
}

ModuleWorker::~ModuleWorker() { }

void ModuleWorker::setupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout(this);
    lyout->addWidget(m_closeButton);

    setLayout(lyout);
}

void ModuleWorker::createCloseButton()
{
    m_closeButton = new QPushButton("Назад", this);
    connect(m_closeButton, &QPushButton::clicked, this, &ModuleWorker::finishWork);
}

bool ModuleWorker::connectToUSB()
{ 
    return m_usb->connect();
}