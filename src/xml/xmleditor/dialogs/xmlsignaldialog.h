#ifndef XMLSIGNALDIALOG_H
#define XMLSIGNALDIALOG_H

#include "xmldialog.h"

class XmlSignalDialog : public XmlDialog
{
    Q_OBJECT
private:
    QLineEdit *idInput, *addrInput, *countInput;

public:
    explicit XmlSignalDialog(XmlSortProxyModel *model, QWidget *parent);
    virtual void setupUI(QStringList &selectedData) override;
    virtual QStringList collectData() override;
};

#endif // XMLSIGNALDIALOG_H
