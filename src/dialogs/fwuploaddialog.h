#ifndef FWUPDIALOG_H
#define FWUPDIALOG_H

#include "../s2/hexparser.h"
#include "../widgets/udialog.h"

#include <gen/error.h>

enum class FirmwareUploadStatus : quint8
{
    Start = 0,
    Error,
    Written,
    Upgraded,
    End,
};

class FWUploadDialog : public UDialog
{
    Q_OBJECT
private:
    FirmwareUploadStatus uploadStatus;
    S2::HexParser *parser;

public:
    explicit FWUploadDialog(QWidget *parent = nullptr);
    void setupUI();
    bool writeCheckPassword();
    virtual void updateGeneralResponse(const QVariant &msg) override;

public slots:
    void loadFirmware();
    void errorHandle(S2::HexParseError error);
};

#endif // FWUPDIALOG_H
