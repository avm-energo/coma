#pragma once

#include <gen/error.h>
#include <s2/hexparser.h>
#include <widgets/udialog.h>

enum class FirmwareUploadStatus : quint8
{
    Start = 0,
    Error,
    Written,
    Upgraded,
    End,
};

class FWUploadDialog final : public UDialog
{
    Q_OBJECT
private:
    FirmwareUploadStatus uploadStatus;
    S2::HexParser *parser;

public:
    explicit FWUploadDialog(Device::CurrentDevice *device, QWidget *parent = nullptr);
    void setupUI();
    bool writeCheckPassword();
    virtual void updateGeneralResponse(const DataTypes::GeneralResponseStruct &response) override;

public slots:
    void loadFirmware();
    void errorHandle(S2::HexParseError error);
};
