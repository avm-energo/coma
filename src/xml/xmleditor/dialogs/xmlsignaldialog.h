#ifndef XMLSIGNALDIALOG_H
#define XMLSIGNALDIALOG_H

#include "xmldialog.h"

class XmlSignalDialog : public XmlDialog
{
    Q_OBJECT
private:
    QComboBox *type;

public:
    explicit XmlSignalDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
    virtual QStringList collectData() override;

public slots:
    virtual void modelDataResponse(const QStringList &response) override;
};

#endif // XMLSIGNALDIALOG_H
