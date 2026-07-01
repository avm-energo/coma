#pragma once

#include <interfaces/ifaces/usbhidport.h>

#include <QPushButton>
#include <QWidget>

class ModuleWorker : public QWidget
{
    Q_OBJECT

public:
    explicit ModuleWorker(bool isDry, UsbHidSettings *usb, QWidget *parent = nullptr);
    virtual ~ModuleWorker();

private:
    bool m_isDry = false;
    UsbHidPort *m_usb = nullptr;

    QPushButton *m_closeButton = nullptr;

    void setupUI();
    void createCloseButton();
    bool connectToUSB();

signals:
    void finishWork();
};