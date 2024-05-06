#include "xml/xmleditor/models/baseeditormodel.h"

BaseEditorModel::BaseEditorModel(int rows, int cols, ModelType type, QObject *parent)
    : QStandardItemModel(rows, cols, parent), m_type(type)
{
}

void BaseEditorModel::setHorizontalHeaderLabels(const QStringList &labels)
{
    int column = 0;
    for (auto &label : labels)
    {
        setHeaderData(column, Qt::Horizontal, label);
        column++;
    }
}

ModelType BaseEditorModel::getModelType() const
{
    return m_type;
}

void BaseEditorModel::setAttribute(
    QDomDocument &doc, QDomElement &elem, const QString &attrName, const QVariant &attrVar)
{
    auto attr = doc.createAttribute(attrName);
    auto attrVal = (attrVar.isValid() && attrVar.canConvert<QString>()) ? attrVar.value<QString>() : "";
    attr.setValue(attrVal);
    elem.setAttributeNode(attr);
}

QDomElement BaseEditorModel::makeElement(QDomDocument &doc, const QString &elemName)
{
    return doc.createElement(elemName);
}

void BaseEditorModel::makeElement(QDomDocument &doc, QDomElement &parent, const QString &elemName, const QString &data)
{
    auto elem = makeElement(doc, elemName);
    auto filler = doc.createTextNode(data);
    elem.appendChild(filler);
    parent.appendChild(elem);
}

void BaseEditorModel::makeElement(QDomDocument &doc, QDomElement &parent, const QString &elemName, const QVariant &data)
{
    auto str = (data.isValid() && data.canConvert<QString>()) ? data.value<QString>() : "";
    makeElement(doc, parent, elemName, str);
}

QStringList BaseEditorModel::getRowData(const int row)
{
    QStringList retList;
    if (row >= 0 && row < rowCount())
    {
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
    }
    return retList;
}

void BaseEditorModel::create(const QStringList &saved, int *row)
{
    *row = rowCount();
    QList<QStandardItem *> items;
    for (const auto &item : saved)
        items.append(new QStandardItem(item));
    insertRow(rowCount(), items);
}

void BaseEditorModel::update(const QStringList &saved, const int row)
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

void BaseEditorModel::remove(const int row)
{
    removeRow(row);
}
