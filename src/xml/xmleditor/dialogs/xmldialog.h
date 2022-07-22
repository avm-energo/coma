#ifndef XMLDIALOG_H
#define XMLDIALOG_H

#include "../models/xmlsortproxymodel.h"

#include <QDialog>

class XmlDialog : public QDialog
{
    Q_OBJECT

protected:
    XmlSortProxyModel *mModel;
    bool isChanged;
    int mRow;

    QStringList getSelectedData();
    void setupSizePos(int width, int height);

public:
    explicit XmlDialog() = delete;
    explicit XmlDialog(XmlSortProxyModel *model, QWidget *parent);
    virtual void reject() override;

    void setupUICall(int &row);
    virtual void writeData(QStringList &saved);

    /// \brief Виртуальный метод для создания UI диалога (виджетов)
    virtual void setupUI(QStringList &selectedData) = 0;
    /// \brief Виртуальный метод для сбора информации с виджетов при сохранении данных
    virtual QStringList collectData() = 0;

public slots:
    void dataChanged();
    void dataChanged(const QString &strData);
    void saveData();
};

#endif // XMLDIALOG_H
