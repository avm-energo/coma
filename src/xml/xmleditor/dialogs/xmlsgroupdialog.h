#ifndef XMLSGROUPDIALOG_H
#define XMLSGROUPDIALOG_H

#include "xmldialog.h"

class XmlSGroupDialog : public XmlDialog
{
    Q_OBJECT
private:
    QLineEdit *nameInput, *idTab;

public:
    XmlSGroupDialog(XmlSortProxyModel *model, QWidget *parent);
    virtual void setupUI(QStringList &selectedData) override;
    virtual QStringList collectData() override;
};

#endif // XMLSGROUPDIALOG_H
