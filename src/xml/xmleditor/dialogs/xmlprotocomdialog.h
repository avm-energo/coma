#ifndef XMLPROTOCOMDIALOG_H
#define XMLPROTOCOMDIALOG_H

#include "xmldialog.h"

class XmlProtocomDialog : public XmlDialog
{
    Q_OBJECT
public:
    XmlProtocomDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};

#endif // XMLPROTOCOMDIALOG_H
