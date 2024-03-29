#pragma once

#include <xml/xmleditor/dialogs/xmldialog.h>

class ModuleDialog : public XmlDialog
{
    Q_OBJECT
private:
    QList<QPair<QString, QString>> dlgSettings;

public:
    explicit ModuleDialog(QWidget *parent);
    virtual void setupUI(QVBoxLayout *mainLayout) override;
    virtual QStringList collectData() override;

public slots:
    virtual void modelDataResponse(const QStringList &response) override;
};
