#ifndef FWUPDIALOG_H
#define FWUPDIALOG_H

#include "../gen/error.h"
#include "../widgets/udialog.h"

class FWUploadDialog : public UDialog
{
    Q_OBJECT
public:
    explicit FWUploadDialog(QWidget *parent = nullptr);

    void SetupUI();
    bool ParseHexToS2(QByteArray ba);
    bool WriteCheckPassword();

    QByteArray *BaForSend;

    bool ok;

public slots:
    void LoadFW();
    void RunSoft();
};

#endif // FWUPDIALOG_H
