#pragma once

#include <comawidgets/udialog.h>

class InfoDialog final : public UDialog
{
    Q_OBJECT
public:
    explicit InfoDialog(Device::CurrentDevice *device, QWidget *parent = nullptr);

private:
    void setupUI();
    void uponInterfaceSetting() override;
    void reqUpdate() override;

    void fillBsi();
    void fillBsiExt(u32 addr);

private slots:
    void sync();
    void syncExt(u32 addr);
    void reqBsi();
    void reqBsiExt();

signals:
    void fetchBsi();
};
