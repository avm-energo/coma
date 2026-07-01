#pragma once

#include <interfaces/types/usbhidportinfo.h>

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
    UsbHidSettings *m_usb = nullptr;

    QPushButton *m_closeButton = nullptr;

    void setupUI();
    void createCloseButton();
    void connectToUSB();

signals:
    void finishWork();
};