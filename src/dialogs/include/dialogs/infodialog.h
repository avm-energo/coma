#pragma once

#include <widgets/udialog.h>

class InfoDialog final : public UDialog
{
    Q_OBJECT
public:
    explicit InfoDialog(Device::CurrentDevice *device, QWidget *parent = nullptr);

private:
    void setupData();
    void setupUI();
    void uponInterfaceSetting() override;
    void reqUpdate() override;

    void fillBsi();
    void fillBsiExt();

private slots:
    void sync();
    void syncExt();

signals:
    void fetchBsi();
};
