#ifndef XMLSIGNALDIALOG_H
#define XMLSIGNALDIALOG_H

#include "xmldialog.h"

class XmlSignalDialog : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlSignalDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};

#endif // XMLSIGNALDIALOG_H
