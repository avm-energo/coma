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

XmlModel::XmlModel(int rows, int cols, ModelType type, QObject *parent)
    : QAbstractTableModel(parent), mRows(rows), mCols(cols), mType(type)
{
    horizontalHeaders.reserve(mCols);
}

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
                if (val.canConvert<ModelNode>())
                    mNodes.insert(index.row(), val);
            }
        }
    }
    return false;
}

int XmlModel::rowCount([[maybe_unused]] const QModelIndex &index) const
{
    return mRows;
}

int XmlModel::columnCount([[maybe_unused]] const QModelIndex &index) const
{
    return mCols;
}

Qt::ItemFlags XmlModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    return index.isValid() ? (flags | Qt::ItemIsEditable) : flags;
}

bool XmlModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    // Первоначальная проверка
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

QVariant XmlModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Horizontal && role == Qt::DisplayRole)
    {
        if (section >= 0 && section < mCols)
            return horizontalHeaders.value(section, QVariant());
    }
    return QVariant();
}

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

void XmlModel::parseDataNode(QDomNode &child, int &row)
{
    auto childNodeName = child.nodeName();
    auto type = types.find(childNodeName);
    if (type != types.cend())
    {
        auto modelNode = ModelNode { nullptr, type->second };
        ModelFabric::CreateModel(modelNode, child, this);
        auto itemIndex = index(row, 0);
        setData(itemIndex, QVariant::fromValue(modelNode), ModelNodeRole);
    }
    parseNode(child, row);
    row++;
}

ModelType XmlModel::getModelType() const
{
    return mType;
}

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
            bool isModelNull = node.value<ModelNode>().modelPtr == nullptr,
                 isModelTypeNone = node.value<ModelNode>().modelType == ModelType::None;
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

void XmlModel::parseTag(QDomNode &node, QString tagName, int row, int col)
{
    auto namedNode = node.firstChildElement(tagName);
    if (!namedNode.isNull())
    {
        auto tagIndex = index(row, col);
        auto tag = namedNode.firstChild().toText().data();
        setData(tagIndex, tag);
    }
}

void XmlModel::parseAttribute(QDomNode &node, QString attrName, int row, int col)
{
    auto attr = node.toElement().attribute(attrName, "");
    if (!attr.isNull())
    {
        auto attrIndex = index(row, col);
        setData(attrIndex, attr);
    }
}
