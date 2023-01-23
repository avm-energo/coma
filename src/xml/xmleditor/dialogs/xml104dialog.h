#ifndef XML104DIALOG_H
#define XML104DIALOG_H

#include "xmldialog.h"

class Xml104Dialog : public XmlDialog
{
    Q_OBJECT
public:
    Xml104Dialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
};

#endif // XML104DIALOG_H
