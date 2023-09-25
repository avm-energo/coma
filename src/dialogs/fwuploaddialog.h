#ifndef FWUPDIALOG_H
#define FWUPDIALOG_H

#include "../widgets/udialog.h"

#include <gen/error.h>
#include <s2/hexparser.h>

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
    virtual void updateGeneralResponse(const DataTypes::GeneralResponseStruct &response) override;

public slots:
    void loadFirmware();
    void errorHandle(S2::HexParseError error);
};

#endif // FWUPDIALOG_H
