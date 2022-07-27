#ifndef XMLMWIDGETDIALOG_H
#define XMLMWIDGETDIALOG_H

#include "xmldialog.h"

class XmlMWidgetDialog : public XmlDialog
{
    Q_OBJECT
private:
    QLineEdit *desc, *startAddr, *count, *tooltip, *strArray;

public:
    XmlMWidgetDialog(XmlSortProxyModel *model, QWidget *parent);
    virtual void setupUI(QStringList &selectedData) override;
    virtual QStringList collectData() override;

public slots:
    virtual void saveData() override;
};

#endif // XMLMWIDGETDIALOG_H
