#ifndef XMLCONFIGDIALOG_H
#define XMLCONFIGDIALOG_H

#include "xmldialog.h"

class XmlConfigDialog : public XmlDialog
{
    Q_OBJECT
private:
    QLineEdit *idInput, *defaultInput;

public:
    XmlConfigDialog(XmlSortProxyModel *model, QWidget *parent);
    virtual void setupUI(QStringList &selectedData) override;
    virtual QStringList collectData() override;
};

#endif // XMLCONFIGDIALOG_H
