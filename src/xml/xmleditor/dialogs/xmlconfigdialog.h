#ifndef XMLCONFIGDIALOG_H
#define XMLCONFIGDIALOG_H

#include "xmldialog.h"

class XmlConfigDialog : public XmlDialog
{
    Q_OBJECT
public:
    XmlConfigDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};

#endif // XMLCONFIGDIALOG_H
