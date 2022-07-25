#ifndef XMLMODBUSDIALOG_H
#define XMLMODBUSDIALOG_H

#include "xmldialog.h"

#include <QComboBox>

class XmlModbusDialog : public XmlDialog
{
    Q_OBJECT
private:
    QLineEdit *regTypeInput, *sigIdInput, *descInput;
    QComboBox *type;

public:
    XmlModbusDialog(XmlSortProxyModel *model, QWidget *parent);
    virtual void setupUI(QStringList &selectedData) override;
    virtual QStringList collectData() override;
};

#endif // XMLMODBUSDIALOG_H
