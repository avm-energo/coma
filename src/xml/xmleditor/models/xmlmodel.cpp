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
XmlModel::XmlModel(int rows, int cols, ModelType type, QObject *parent) : IEditorModel(rows, cols, type, parent)
{
}

/*! \brief Returns the data stored under the given role for the item referred to by the index.
 *  \details This function reimplement and override the same
 *  function from base class QAbstractItemModel.
 *  \see setData, setHeaderData
 */
QVariant XmlModel::data(const QModelIndex &index, int nRole) const
{
    if (index.isValid())
        return QStandardItemModel::data(index, nRole);
    else
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
        if ((column >= 0 && column < columnCount()) && (row >= 0 && row < rowCount()))
            return QStandardItemModel::setData(index, val, nRole);
    }
    return false;
}

void XmlModel::create(const QStringList &saved, int *row)
{
    IEditorModel::create(saved, row);
    emit modelChanged();
}

void XmlModel::update(const QStringList &saved, const int &row)
{
    IEditorModel::update(saved, row);
    emit modelChanged();
}

void XmlModel::remove(const int &row)
{
    IEditorModel::remove(row);
    emit modelChanged();
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
