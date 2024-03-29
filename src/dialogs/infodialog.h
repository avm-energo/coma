#ifndef INFODIALOG_H
#define INFODIALOG_H

#include "../widgets/udialog.h"

class InfoDialog : public UDialog
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

#endif // INFODIALOG_H
