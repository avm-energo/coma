#ifndef XMLMODBUSDIALOG_H
#define XMLMODBUSDIALOG_H

#include "xmldialog.h"

class XmlModbusDialog : public XmlDialog
{
    Q_OBJECT
private:
    QComboBox *type;

public:
    XmlModbusDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
    virtual QStringList collectData() override;

public slots:
    virtual void modelDataResponse(const QStringList &response) override;
};

#endif // XMLMODBUSDIALOG_H
