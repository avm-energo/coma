#ifndef XMLSECTIONDIALOG_H
#define XMLSECTIONDIALOG_H

#include "xmldialog.h"

class XmlSectionDialog : public XmlDialog
{
    Q_OBJECT
private:
    QLineEdit *nameInput;

public:
    XmlSectionDialog(XmlSortProxyModel *model, QWidget *parent);
    virtual void setupUI(QStringList &selectedData) override;
    virtual QStringList collectData() override;
};

#endif // XMLSECTIONDIALOG_H
