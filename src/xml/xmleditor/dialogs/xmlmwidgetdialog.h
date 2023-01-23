#ifndef XMLMWIDGETDIALOG_H
#define XMLMWIDGETDIALOG_H

#include "xmldialog.h"

class XmlMWidgetDialog : public XmlDialog
{
    Q_OBJECT
private:
    QComboBox *view;

public:
    XmlMWidgetDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
    virtual QStringList collectData() override;

public slots:
    virtual void modelDataResponse(const QStringList &response) override;
};

#endif // XMLMWIDGETDIALOG_H
