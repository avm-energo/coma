#ifndef XMLMWIDGETDIALOG_H
#define XMLMWIDGETDIALOG_H

#include "xmldialog.h"

class XmlMWidgetDialog : public XmlDialog
{
    Q_OBJECT
public:
    XmlMWidgetDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};

#endif // XMLMWIDGETDIALOG_H
