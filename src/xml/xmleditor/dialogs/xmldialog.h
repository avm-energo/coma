#ifndef XMLDIALOG_H
#define XMLDIALOG_H

#include "../models/xmlsortproxymodel.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegExpValidator>

constexpr auto createId = -1;

class XmlDialog : public QDialog
{
    Q_OBJECT
protected:
    QString mTitle;
    XmlSortProxyModel *mModel;
    bool isChanged;
    int mRow;

    QStringList getSelectedData();
    void setupSizePos(int width, int height);
    void addSaveBtnAndApply(QVBoxLayout *mainLayout);

public:
    explicit XmlDialog() = delete;
    explicit XmlDialog(XmlSortProxyModel *model, QWidget *parent);
    void setupUICall(int &row);
    void writeData(QStringList &saved);
    void setTitle(const QString title);
    virtual void reject() override;

    /// \brief Виртуальный метод для создания UI диалога (виджетов)
    virtual void setupUI(QStringList &selectedData) = 0;
    /// \brief Виртуальный метод для сбора информации с виджетов при сохранении данных
    virtual QStringList collectData() = 0;

public slots:
    void dataChanged();
    void dataChanged(const QString &strData);
    virtual void saveData();
};

#endif // XMLDIALOG_H
