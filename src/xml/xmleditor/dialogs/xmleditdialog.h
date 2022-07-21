#ifndef XMLEDITDIALOG_H
#define XMLEDITDIALOG_H

#include "../models/xmlsortproxymodel.h"

#include <QDialog>

class XmlEditDialog : public QDialog
{
    Q_OBJECT
private:
    QStringList getSelectedData();

protected:
    XmlSortProxyModel *mModel;
    bool isChanged;
    int mRow;

public:
    explicit XmlEditDialog() = delete;
    explicit XmlEditDialog(XmlSortProxyModel *model, QWidget *parent);
    virtual void reject() override;

    void setupUICall(int &row);
    virtual void writeData(QStringList saved);

    virtual void setupUI(QStringList &selectedData) = 0;
    virtual QStringList collectData() = 0;

public slots:
    void dataChanged();
    void saveData();
};

#endif // XMLEDITDIALOG_H
