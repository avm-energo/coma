#include "moduleWorker/moduleWorker.h"

#include <QVBoxLayout>

ModuleWorker::ModuleWorker(bool isDry, UsbHidSettings *usb, QWidget *parent)
    : m_isDry(isDry)
    , m_usb(usb)
    , QWidget(parent)
{
    createCloseButton();
    setupUI();
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

void ModuleWorker::connectToUSB() { }