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
    bool WriteCheckPassword();

public slots:
    void LoadFW();
    void RunSoft();
};

#endif // FWUPDIALOG_H
