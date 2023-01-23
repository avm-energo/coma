#ifndef XMLSECTIONDIALOG_H
#define XMLSECTIONDIALOG_H

#include "xmldialog.h"

class XmlSectionDialog : public XmlDialog
{
    Q_OBJECT
public:
    XmlSectionDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};

#endif // XMLSECTIONDIALOG_H
