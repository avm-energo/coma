#ifndef XMLALARMDIALOG_H
#define XMLALARMDIALOG_H

#include "xmldialog.h"

class XmlAlarmDialog : public XmlDialog
{
    Q_OBJECT
public:
    explicit XmlAlarmDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};

#endif // XMLALARMDIALOG_H
