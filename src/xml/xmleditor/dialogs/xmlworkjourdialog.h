#ifndef XMLWORKJOURDIALOG_H
#define XMLWORKJOURDIALOG_H

#include "xmldialog.h"

class XmlWorkJourDialog : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlWorkJourDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};

#endif // XMLWORKJOURDIALOG_H
