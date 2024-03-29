#include "xml/xmleditor/models/baseeditormodel.h"

BaseEditorModel::BaseEditorModel(int rows, int cols, ModelType type, QObject *parent)
    : QStandardItemModel(rows, cols, parent), mType(type)
{
}

/// \brief Sets horizontal header labels of model from an input string list.
void BaseEditorModel::setHorizontalHeaderLabels(const QStringList &labels)
{
    int column = 0;
    for (auto &label : labels)
    {
        setHeaderData(column, Qt::Horizontal, label);
        column++;
    }
}

/// \brief Returns a model type.
ModelType BaseEditorModel::getModelType() const
{
    return mType;
}

/// \brief Creates an attribute at base of input data.
/// \param doc - XML document in that creates data.
/// \param elem - XML DOM node, child of doc. New attribute creates for this node.
/// \param attrName - a name of creating attribute.
/// \param attrVar - QVariant with string value of creating attribute.
void BaseEditorModel::setAttribute(
    QDomDocument &doc, QDomElement &elem, const QString &attrName, const QVariant &attrVar)
{
    auto attr = doc.createAttribute(attrName);
    auto attrVal = (attrVar.isValid() && attrVar.canConvert<QString>()) ? attrVar.value<QString>() : "";
    attr.setValue(attrVal);
    elem.setAttributeNode(attr);
}

/// \brief Creates QDomNode at base of input QDomDocument.
QDomElement BaseEditorModel::makeElement(QDomDocument &doc, const QString &elemName)
{
    return doc.createElement(elemName);
}

/// \brief Creates a QDomNode at base of input data.
/// \param doc - XML document in that creates data.
/// \param parent - XML DOM node, parent of creating node.
/// \param elemName - name of creating node.
/// \param data - string data for the new node.
void BaseEditorModel::makeElement(QDomDocument &doc, QDomElement &parent, const QString &elemName, const QString &data)
{
    auto elem = makeElement(doc, elemName);
    auto filler = doc.createTextNode(data);
    elem.appendChild(filler);
    parent.appendChild(elem);
}

/// \brief Creates a QDomNode at base of input data.
/// \param doc - XML document in that creates data.
/// \param parent - XML DOM node, parent of creating node.
/// \param elemName - name of creating node.
/// \param data - QVariant with string data for the new node.
void BaseEditorModel::makeElement(QDomDocument &doc, QDomElement &parent, const QString &elemName, const QVariant &data)
{
    auto str = (data.isValid() && data.canConvert<QString>()) ? data.value<QString>() : "";
    makeElement(doc, parent, elemName, str);
}

/// \brief Slot for receiving a request from dialog and emits signal with response.
void BaseEditorModel::getDialogRequest(const int row)
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
        // Send response
        emit sendDialogResponse(retList);
    }
}

/// \brief Slot for inserting a new item in the model.
void BaseEditorModel::create(const QStringList &saved, int *row)
{
    *row = rowCount();
    QList<QStandardItem *> items;
    for (const auto &item : saved)
        items.append(new QStandardItem(item));
    insertRow(rowCount(), items);
}

/// \brief Slot for updating an item's data in the model.
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

/// \brief Slot for deleting an exisiting item in the model.
void BaseEditorModel::remove(const int row)
{
    removeRow(row);
}
