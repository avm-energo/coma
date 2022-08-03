#include "ieditormodel.h"

IEditorModel::IEditorModel(int rows, int cols, ModelType type, QObject *parent)
    : QStandardItemModel(rows, cols, parent), mType(type)
{
}

void IEditorModel::setHorizontalHeaderLabels(const QStringList &labels)
{
    int column = 0;
    for (auto &label : labels)
    {
        setHeaderData(column, Qt::Horizontal, label);
        column++;
    }
}

ModelType IEditorModel::getModelType() const
{
    return mType;
}

/// \brief Слот который принимает запрос от диалога и отправляет сигнал с ответом
void IEditorModel::getDialogRequest(const int &row)
{
    if (row >= 0 && row < rowCount())
    {
        QStringList retList;
        auto cols = columnCount();
        retList.reserve(cols);
        // Собираем данные
        for (auto column = 0; column < cols; column++)
        {
            auto itemIndex = index(row, column);
            auto item = data(itemIndex);
            if (item.isValid() && item.canConvert<QString>())
            {
                auto itemStr = item.value<QString>();
                retList.append(itemStr);
            }
            else
                retList.append("");
        }
        // Отправляем сигнал с ответом
        emit sendDialogResponse(retList);
    }
}

void IEditorModel::create(const QStringList &saved, int *row)
{
    *row = rowCount();
    QList<QStandardItem *> items;
    for (const auto &item : saved)
        items.append(new QStandardItem(item));
    insertRow(rowCount(), items);
}

void IEditorModel::update(const QStringList &saved, const int &row)
{
    if (row >= 0 && row < rowCount())
    {
        auto cols = saved.count();
        // Обновляем данные
        for (auto column = 0; column < cols; column++)
        {
            auto itemIndex = index(row, column);
            setData(itemIndex, saved[column]);
        }
    }
}

void IEditorModel::remove(const int &row)
{
    removeRow(row);
}
