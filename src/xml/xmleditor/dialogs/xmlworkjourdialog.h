#ifndef XMLWORKJOURDIALOG_H
#define XMLWORKJOURDIALOG_H

#include "xmldialog.h"

class XmlWorkJourDialog : public XmlDialog
{
    Q_OBJECT
private:
    QLineEdit *addrInput, *descInput;

public:
    explicit XmlWorkJourDialog(XmlSortProxyModel *model, QWidget *parent);
    virtual void setupUI(QStringList &selectedData) override;
    virtual QStringList collectData() override;
};

#endif // XMLWORKJOURDIALOG_H
