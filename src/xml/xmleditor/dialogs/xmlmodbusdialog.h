#ifndef XMLMODBUSDIALOG_H
#define XMLMODBUSDIALOG_H

#include "xmldialog.h"

class XmlModbusDialog : public XmlDialog
{
    Q_OBJECT
public:
    XmlModbusDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};

#endif // XMLMODBUSDIALOG_H
