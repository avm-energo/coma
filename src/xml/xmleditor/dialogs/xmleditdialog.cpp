#include "xmleditdialog.h"

#include <QLayout>
#include <QMessageBox>

XmlEditDialog::XmlEditDialog(XmlSortProxyModel *model, QWidget *parent)
    : QDialog(parent, Qt::Window), mModel(model), isChanged(false), mRow(0)
{
}

void XmlEditDialog::setupUICall(int &row)
{
    mRow = row;
    auto selectedData = getSelectedData();
    setupUI(selectedData);
    this->exec();
}

void XmlEditDialog::reject()
{
    if (isChanged)
    {
        auto resBtn = QMessageBox::question(this, "Сохранение", tr("Сохранить изменения?\n"),
            QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
        if (resBtn == QMessageBox::Yes)
        {
            saveData();
        }
    }
    QDialog::reject();
}

QStringList XmlEditDialog::getSelectedData()
{
    QStringList selectedData;
    auto cols = mModel->columnCount();
    for (auto i = 0; i < cols; i++)
    {
        auto index = mModel->index(mRow, i);
        selectedData.append(mModel->data(index).value<QString>());
    }
    return selectedData;
}

void XmlEditDialog::writeData(QStringList saved)
{
    auto cols = mModel->columnCount();
    if (cols == saved.size())
    {
        for (auto i = 0; i < cols; i++)
        {
            auto index = mModel->index(mRow, i);
            mModel->setData(index, saved[i]);
        }
    }
}

void XmlEditDialog::dataChanged()
{
    if (!isChanged)
        setWindowTitle(windowTitle() + " [ИЗМЕНЕНО]");
    isChanged = true;
}

void XmlEditDialog::saveData()
{
    if (isChanged)
    {
        writeData(collectData());
        isChanged = false;
        auto curTitle = windowTitle();
        curTitle = curTitle.left(curTitle.lastIndexOf(" [ИЗМЕНЕНО]"));
        setWindowTitle(curTitle);
    }
}
