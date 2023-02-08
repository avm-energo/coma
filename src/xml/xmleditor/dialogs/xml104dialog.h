#pragma once

#include "xmldialog.h"

class Xml104Dialog : public XmlDialog
{
    Q_OBJECT
private:
    QString convertStringToType(const QString &namedTransType);
    int convertTypeToIndex(const QString &transTypeNum);

public:
    Xml104Dialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
    virtual QStringList collectData() override;

public slots:
    virtual void modelDataResponse(const QStringList &response) override;
};
