#include "xmlmodel.h"

#include "modelfabric.h"

const std::map<QString, ModelType> XmlModel::types {
    { tags::res, ModelType::Resources },       //
    { tags::sigs, ModelType::Signals },        //
    { tags::tabs, ModelType::SectionTabs },    //
    { tags::sections, ModelType::Sections },   //
    { tags::section, ModelType::Section },     //
    { tags::sgroup, ModelType::SGroup },       //
    { tags::alarms, ModelType::Alarms },       //
    { tags::critical, ModelType::AlarmsItem }, //
    { tags::warning, ModelType::AlarmsItem },  //
    { tags::info, ModelType::AlarmsItem },     //
    { tags::journals, ModelType::Journals },   //
    { tags::work, ModelType::WorkJours },      //
    { tags::meas, ModelType::MeasJours },      //
    { tags::modbus, ModelType::Modbus },       //
    { tags::protocom, ModelType::Protocom },   //
    { tags::iec60870, ModelType::IEC60870 },   //
    { tags::config, ModelType::Config }        //
};

const std::map<ModelType, QStringList> XmlModel::headers {
    { ModelType::Resources, { "XML", "Описание" } },                                         //
    { ModelType::Signals, { "ID сигнала", "Стартовый адрес", "Количество" } },               //
    { ModelType::SectionTabs, { "ID вкладки", "Название" } },                                //
    { ModelType::Sections, { "Название" } },                                                 //
    { ModelType::Section, { "Название", "ID вкладки" } },                                    //
    { ModelType::SGroup, { "Имя", "Адрес" } },                                               //
    { ModelType::Alarms, { "XML", "Описание" } },                                            //
    { ModelType::AlarmsItem, { "Описание", "Адрес" } },                                      //
    { ModelType::Journals, { "XML", "Описание" } },                                          //
    { ModelType::WorkJours, { "Адрес", "Описание" } },                                       //
    { ModelType::MeasJours, { "Название" } },                                                //
    { ModelType::Modbus, { "ID сигнала", "Тип регистра", "Возвращаемый тип", "Описание" } }, //
    { ModelType::Protocom, { "Блок", "ID сигнала" } },                                       //
    { ModelType::IEC60870, { "ID сигнала", "Тип сигнала", "Тип передачи", "Группа" } },      //
    { ModelType::Config, { "ID виджета", "Значение по умолчанию" } }                         //
};

/// \brief Base XML model class ctor
XmlModel::XmlModel(int rows, int cols, ModelType type, QObject *parent)
    : QAbstractTableModel(parent), mRows(rows), mCols(cols), mType(type)
{
    horizontalHeaders.reserve(mCols);
}

/*! \brief Returns the data stored under the given role for the item referred to by the index.
 *  \details This function reimplement and override the same
 *  function from base class QAbstractItemModel.
 *  \see setData, setHeaderData
 */
QVariant XmlModel::data(const QModelIndex &index, int nRole) const
{
    if (index.isValid())
    {
        if (nRole == Qt::DisplayRole || nRole == Qt::EditRole || nRole == (Qt::UserRole + 1))
            return mHashTable.value(index, QString(""));
        else if (nRole == ModelNodeRole)
            return mNodes.value(index.row(), QVariant());
    }
    return QVariant();
}

/*! \brief Sets the role data for the item at index to value.
 *  \details This function reimplement and override the same
 *  function from base class QAbstractItemModel.
 *  \returns Returns true if successful; otherwise returns false.
 *  \see data, headerData
 */
bool XmlModel::setData(const QModelIndex &index, const QVariant &val, int nRole)
{
    if (index.isValid() && val.isValid())
    {
        auto column = index.column();
        auto row = index.row();
        if ((column >= 0 && column < mCols) && (row >= 0 && row < mRows))
        {
            if (nRole == Qt::EditRole || nRole == (Qt::UserRole + 1))
            {
                mHashTable.insert(index, val);
                emit dataChanged(index, index);
            }
            else if (nRole == ModelNodeRole)
            {
                if (val.canConvert<ChildModelNode>())
                    mNodes.insert(index.row(), val);
            }
        }
    }
    return false;
}

/*! \brief Returns the number of rows in current XML model state.
 *  \details This function reimplement and override the same
 *  function from base class QAbstractItemModel.
 *  \see columnCount
 */
int XmlModel::rowCount([[maybe_unused]] const QModelIndex &index) const
{
    return mRows;
}

/*! \brief Returns the number of columns in current XML model state.
 *  \details This function reimplement and override the same
 *  function from base class QAbstractItemModel.
 *  \see rowCount
 */
int XmlModel::columnCount([[maybe_unused]] const QModelIndex &index) const
{
    return mCols;
}

/*! \brief Returns the item flags for the given index.
 *  \details This function reimplement and override the same
 *  function from base class QAbstractItemModel.
 */
Qt::ItemFlags XmlModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    return index.isValid() ? (flags | Qt::ItemIsEditable) : flags;
}

/*! \brief Sets the data for the given role and section in the header with the specified orientation.
 *  \details This function reimplement and override the same function from base
 *  class QAbstractItemModel. Current function support only Qt::Horizontal orientation.
 *  \see headerData, setData
 */
bool XmlModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    // First check
    if ((section < 0) //
        || ((orientation == Qt::Horizontal) && (section >= columnCount()))
        || ((orientation == Qt::Vertical) && (section >= rowCount())))
    {
        return false;
    }

    if (orientation == Qt::Horizontal && role == Qt::EditRole)
    {
        horizontalHeaders.insert(section, value);
        return true;
    }
    return false;
}

/*! \brief Returns the data for the given role and section in the header with the specified orientation.
 *  \details This function reimplement and override the same function from base
 *  class QAbstractItemModel. Current function support only Qt::Horizontal orientation.
 *  \see setHeaderData, data
 */
QVariant XmlModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section >= 0 && section < mCols)
            return horizontalHeaders.value(section, QVariant());
    }
    return QVariant();
}

/*! \brief Sets horizontal headers from given labels.
 *  \details Sets horizontal header labels from given string list in the header structure.
 */
void XmlModel::setHorizontalHeaderLabels(const QStringList &labels)
{
    if (labels.count() > columnCount())
        mRows = labels.count();

    int column = 0;
    bool status = false;
    for (auto &label : labels)
    {
        status = setHeaderData(column, Qt::Horizontal, label);
        Q_ASSERT(status);
        column++;
    }
}

/*! \brief Parses given XML DOM node in current XML model .
 *  \details For each child node of given XML DOM node applying function parseDataNode.
 *  \see parseDataNode, parseNode
 */
void XmlModel::setDataNode(bool isChildModel, QDomNode &root)
{
    int row = 0;
    if (isChildModel)
    {
        auto itemIndex = index(row, 0);
        setData(itemIndex, QString(".."));
        row++;
    }
    auto child = root.firstChild();
    while (!child.isNull())
    {
        if (!child.isComment())
            parseDataNode(child, row);
        child = child.nextSibling();
    }
}

/*! \brief Parses given child node in current XML model.
 *  \details For given child node applying virtual function parseNode. If name
 *  of given node is in static types map, then for this item will be creating new
 *  XML model with help static members of ModelFabric class.
 *  \see parseNode, setDataNode
 */
void XmlModel::parseDataNode(QDomNode &child, int &row)
{
    auto childNodeName = child.nodeName();
    auto type = types.find(childNodeName);
    if (type != types.cend())
    {
        auto modelNode = ChildModelNode { nullptr, type->second };
        ModelFabric::CreateChildModel(modelNode, child, this);
        auto itemIndex = index(row, 0);
        setData(itemIndex, QVariant::fromValue(modelNode), ModelNodeRole);
    }
    parseNode(child, row);
    row++;
}

/// \brief Returns model type of current XML model.
ModelType XmlModel::getModelType() const
{
    return mType;
}

/*! \brief Appends new items in current XML model.
 *  \details Appends in the end of current model new row of items with calling
 *  beginInsertRows and endInsertRows functions of QAbstractItemModel.
 *  \see setData, remove
 */
const QModelIndex XmlModel::append(const QStringList &input)
{
    if (input.size() == mCols)
    {
        auto last = mRows;
        beginInsertRows(QModelIndex(), last, last);
        mRows++;
        for (auto i = 0; i < mCols; i++)
        {
            auto itemIndex = index(last, i);
            setData(itemIndex, input[i]);
        }
        endInsertRows();
        return index(last, 0);
    }
    else
        return index(-1, -1);
}

/*! \brief Removes items at given row in current XML model.
 *  \details Removes items at given row from current model with calling
 *  beginRemoveRows, beginMoveRows, endMoveRows and endRemoveRows functions.
 *  \see append
 */
bool XmlModel::remove(int row)
{
    if (row >= 0 && row < mRows)
    {
        beginRemoveRows(QModelIndex(), row, row);
        // Удаляем строку
        for (auto i = 0; i < mCols; i++)
        {
            auto itemIndex = index(row, i);
            mHashTable.remove(itemIndex);
        }
        // Удаляем подмодель, если она есть
        mNodes.remove(row);
        auto last = mRows - 1;
        // Если строка была не последней
        if (row != last)
        {
            // То перемещаем последнюю строку на место удалённой
            beginMoveRows(QModelIndex(), last, last, QModelIndex(), row);
            for (auto i = 0; i < mCols; i++)
            {
                auto srcIndex = index(last, i);
                auto dstIndex = index(row, i);
                mHashTable.insert(dstIndex, mHashTable.take(srcIndex));
            };
            // Если за ней закреплена подмодель, то переносим и её
            auto node = mNodes.take(last);
            bool isModelNull = node.value<ChildModelNode>().modelPtr == nullptr,
                 isModelTypeNone = node.value<ChildModelNode>().modelType == ModelType::None;
            if (!isModelNull && !isModelTypeNone)
            {
                mNodes.insert(row, node);
            }
            endMoveRows();
        }
        // Уменьшаем количество строк в модели
        mRows--;
        endRemoveRows();
        return true;
    }
    return false;
}

/*! \brief Parses given tag from given XML DOM node in XML model at given index.
 *  \details Frequently called by implementations of parseNode virtual function.
 *  \see parseAttribute, parseNode
 */
void XmlModel::parseTag(QDomNode &node, const QString tagName, int row, int col)
{
    auto namedNode = node.firstChildElement(tagName);
    if (!namedNode.isNull())
    {
        auto tagIndex = index(row, col);
        auto tag = namedNode.firstChild().toText().data();
        setData(tagIndex, tag);
    }
}

/*! \brief Parses given attribute from given XML DOM node in XML model at given index.
 *  \details Frequently called by implementations of parseNode virtual function.
 *  \see parseTag, parseNode
 */
void XmlModel::parseAttribute(QDomNode &node, const QString attrName, int row, int col)
{
    auto attr = node.toElement().attribute(attrName, "");
    if (!attr.isNull())
    {
        auto attrIndex = index(row, col);
        setData(attrIndex, attr);
    }
}
