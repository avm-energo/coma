#ifndef XMLPROTOCOMDIALOG_H
#define XMLPROTOCOMDIALOG_H

#include "xmldialog.h"

class XmlProtocomDialog : public XmlDialog
{
    Q_OBJECT
private:
    QLineEdit *blockInput, *sigIdInput;

public:
    XmlProtocomDialog(XmlSortProxyModel *model, QWidget *parent);
    virtual void setupUI(QStringList &selectedData) override;
    virtual QStringList collectData() override;
};

#endif // XMLPROTOCOMDIALOG_H
