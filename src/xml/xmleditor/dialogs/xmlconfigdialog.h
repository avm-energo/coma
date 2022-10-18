#ifndef XMLCONFIGDIALOG_H
#define XMLCONFIGDIALOG_H

#include "xmldialog.h"

class XmlConfigDialog : public XmlDialog
{
    Q_OBJECT
private:
    QComboBox *visibility;

public:
    XmlConfigDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
    virtual QStringList collectData() override;

public slots:
    virtual void modelDataResponse(const QStringList &response) override;
};

#endif // XMLCONFIGDIALOG_H
