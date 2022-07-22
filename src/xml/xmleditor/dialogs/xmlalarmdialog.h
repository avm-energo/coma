#ifndef XMLALARMDIALOG_H
#define XMLALARMDIALOG_H

#include "xmldialog.h"

class XmlAlarmDialog : public XmlDialog
{
    Q_OBJECT
private:
    QLineEdit *descInput, *addrInput;

public:
    explicit XmlAlarmDialog(XmlSortProxyModel *model, QWidget *parent);
    virtual void setupUI(QStringList &selectedData) override;
    virtual QStringList collectData() override;
};

#endif // XMLALARMDIALOG_H
