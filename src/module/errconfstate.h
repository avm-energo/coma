#pragma once

#include "../widgets/uwidget.h"

#include <QTimer>
#include <gen/stdfunc.h>

enum ErrConfParam : quint32
{
    ConfParseCode = 0,
    InvalidConfId,
    SendedLength,
    RequiredLength,
    ResultWritingConf,
    SendedCRC,
    RequiredCRC,
    HardwareErrorCode
};

class ErrConfState : public UWidget
{
    Q_OBJECT
private:
    UniquePointer<QTimer> requestTimer;

    void setupUI();
    void configureRequest();
    virtual void updateBitStringData(const DataTypes::BitStringStruct &bs) override;
    QString createHexString(quint32 value) const;
    virtual void closeEvent(QCloseEvent *event) override;

public:
    explicit ErrConfState(QWidget *parent = nullptr);
    void show();
};
