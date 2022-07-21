#ifndef XMLEDITALARMDIALOG_H
#define XMLEDITALARMDIALOG_H

#include "xmleditdialog.h"

#include <QLineEdit>

class XmlEditAlarmDialog : public XmlEditDialog
{
    Q_OBJECT
private:
    QLineEdit *descInput, *addrInput;

public:
    explicit XmlEditAlarmDialog(XmlSortProxyModel *model, QWidget *parent);
    virtual void setupUI(QStringList &selectedData) override;
    virtual QStringList collectData() override;

public slots:
    void dataChanged(const QString &strData);
};

#endif // XMLEDITALARMDIALOG_H
