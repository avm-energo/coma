#ifndef FWUPDIALOG_H
#define FWUPDIALOG_H

#include "../widgets/udialog.h"

#include <gen/error.h>

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
