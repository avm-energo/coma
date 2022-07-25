#ifndef XML104DIALOG_H
#define XML104DIALOG_H

#include "xmldialog.h"

class Xml104Dialog : public XmlDialog
{
    Q_OBJECT
private:
    QLineEdit *sigIdInput, *sigTypeInput, *transTypeInput, *sigGroupInput;

public:
    Xml104Dialog(XmlSortProxyModel *model, QWidget *parent);
    virtual void setupUI(QStringList &selectedData) override;
    virtual QStringList collectData() override;
};

#endif // XML104DIALOG_H
