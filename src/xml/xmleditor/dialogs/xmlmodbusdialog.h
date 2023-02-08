#pragma once

#include "xmldialog.h"

class XmlModbusDialog : public XmlDialog
{
    Q_OBJECT
private:
    QString convertStringToType(const QString &namedTransType);
    int convertTypeToIndex(const QString &transTypeNum);

public:
    XmlModbusDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
    virtual QStringList collectData() override;

public slots:
    virtual void modelDataResponse(const QStringList &response) override;
};
