#ifndef XMLSTABDIALOG_H
#define XMLSTABDIALOG_H

#include "xmldialog.h"

class XmlSTabDialog : public XmlDialog
{
    Q_OBJECT
private:
    QLineEdit *idInput, *nameInput;

public:
    explicit XmlSTabDialog(XmlSortProxyModel *model, QWidget *parent);
    virtual void setupUI(QStringList &selectedData) override;
    virtual QStringList collectData() override;
};

#endif // XMLSTABDIALOG_H
