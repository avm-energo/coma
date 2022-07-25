#ifndef XMLMEASJOURDIALOG_H
#define XMLMEASJOURDIALOG_H

#include "xmldialog.h"

class XmlMeasJourDialog : public XmlDialog
{
    Q_OBJECT
private:
    QLineEdit *headInput;

public:
    explicit XmlMeasJourDialog(XmlSortProxyModel *model, QWidget *parent);
    virtual void setupUI(QStringList &selectedData) override;
    virtual QStringList collectData() override;
};

#endif // XMLMEASJOURDIALOG_H
