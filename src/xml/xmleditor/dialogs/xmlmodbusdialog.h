#ifndef XMLMODBUSDIALOG_H
#define XMLMODBUSDIALOG_H

#include "xmldialog.h"

#include <QComboBox>

class XmlModbusDialog : public XmlDialog
{
    Q_OBJECT
private:
    QComboBox *type;

public:
    XmlModbusDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
    virtual QStringList collectData() override;
};

#endif // XMLMODBUSDIALOG_H
