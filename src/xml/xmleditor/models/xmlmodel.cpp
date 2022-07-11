#include "xmlmodel.h"

#include "modelfabric.h"

const std::map<QString, GroupTypes> XmlModel::types {
    { "resources", GroupTypes::Resources }, //
    { "check", GroupTypes::Check },         //
    { "groups", GroupTypes::Groups },       //
    { "signals", GroupTypes::Signals },     //
    { "records", GroupTypes::Records }      //
};

const std::map<GroupTypes, QStringList> XmlModel::settings {
    { GroupTypes::Resources, { "XML", "Описание" } },                                     //
    { GroupTypes::Check, { "XML", "Описание" } },                                         //
    { GroupTypes::Groups, { "Номер группы", "Название" } },                               //
    { GroupTypes::Signals, { "Тип сигнала", "Стартовый адрес", "Количество" } },          //
    { GroupTypes::Records, { "Заголовок", "Описание", "Группа", "Адрес", "Количество" } } //
};

XmlModel::XmlModel(int rows, int cols, QObject *parent) : QAbstractTableModel(parent), mRows(rows), mCols(cols)
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
        {
            auto val = mNodes.value(index.row(), QVariant());
            return val;
        }
    }
    return QVariant();
}

bool XmlModel::setData(const QModelIndex &index, const QVariant &val, int nRole)
{
    if (index.isValid() && val.isValid())
    {
        if ((index.column() < mCols) && (index.row() < mRows))
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
    else if (orientation == Qt::Orientation::Vertical && role == Qt::DisplayRole)
    {
        return QVariant::fromValue(QString::number(section + 1));
    }
    return QVariant();
}

void XmlModel::setHorizontalHeaderLabels(const QStringList &labels)
{
    if (labels.count() > columnCount())
        mRows = labels.count();

    int column = 0;
    for (auto &label : labels)
    {
        setHeaderData(column, Qt::Horizontal, label);
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
        checkChilds(child, row);
        child = child.nextSibling();
    }
}

void XmlModel::checkChilds(QDomNode &child, int &row)
{
    auto childNodeName = child.nodeName();
    auto type = types.find(childNodeName);
    if (type != types.cend())
    {
        auto modelNode = ModelNode { nullptr, type->second };
        auto itemIndex = index(row, 0);
        auto itemHeaderIndex = index(row, 1);
        setData(itemIndex, childNodeName);
        setData(itemHeaderIndex, child.toElement().attribute("header", ""));
        ModelFabric::CreateModel(modelNode, child, this);
        setData(itemIndex, QVariant::fromValue(modelNode), ModelNodeRole);
    }
    else
        parseNode(child, row);
    row++;
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
    if (node.hasAttributes())
    {
        auto headerNode = node.attributes().namedItem(attrName);
        if (!headerNode.isNull())
        {
            auto headerIndex = index(row, col);
            auto header = headerNode.firstChild().toText().data();
            setData(headerIndex, header);
        }
    }
}
