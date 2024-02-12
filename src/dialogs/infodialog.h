#ifndef INFODIALOG_H
#define INFODIALOG_H

#include "../widgets/udialog.h"

#include <QMap>

class InfoDialog : public UDialog
{
    Q_OBJECT
public:
    explicit InfoDialog(Device::CurrentDevice *device, QWidget *parent = nullptr);

    void SetupUI();

    void uponInterfaceSetting() override;
    void reqUpdate() override;

public slots:
    // void ClearBsi();

    void sync();
    void syncExt();

signals:
    void fetchBsi();

private:
    void FillBsi();
    void FillBsiExt();
};

#endif // INFODIALOG_H
