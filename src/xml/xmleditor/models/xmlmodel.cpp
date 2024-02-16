#include "xmlmodel.h"

#include "modelfabric.h"

const std::map<QString, ModelType> XmlModel::types {
    { tags::res, ModelType::Resources },     //
    { tags::sigs, ModelType::Signals },      //
    { tags::tabs, ModelType::SectionTabs },  //
    { tags::sections, ModelType::Sections }, //
    { tags::section, ModelType::Section },   //
    { tags::sgroup, ModelType::SGroup },     //
    { tags::alarms, ModelType::Alarms },     //
    { tags::crit, ModelType::AlarmsCrit },   //
    { tags::warn, ModelType::AlarmsWarn },   //
    { tags::info, ModelType::AlarmsInfo },   //
    { tags::journals, ModelType::Journals }, //
    { tags::work, ModelType::WorkJours },    //
    { tags::meas, ModelType::MeasJours },    //
    { tags::modbus, ModelType::Modbus },     //
    { tags::protocom, ModelType::Protocom }, //
    { tags::iec, ModelType::IEC60870 },      //
    { tags::config, ModelType::Config },     //
    { tags::hidden, ModelType::Hidden },     //
    { tags::hidden_tab, ModelType::HiddenTab },
};

const std::map<ModelType, QStringList> XmlModel::headers {
    { ModelType::Resources, { "XML", "Описание" } },                                                     //
    { ModelType::Signals, { "Стартовый адрес", "Количество", "ID сигнала", "Тип" } },                    //
    { ModelType::SectionTabs, { "ID вкладки", "Название" } },                                            //
    { ModelType::Sections, { "Название" } },                                                             //
    { ModelType::Section, { "Название", "ID вкладки" } },                                                //
    { ModelType::SGroup, { "Адрес", "Имя" } },                                                           //
    { ModelType::Alarms, { "XML", "Описание" } },                                                        //
    { ModelType::AlarmsCrit, { "Адрес", "Описание", "Подсветка" } },                                     //
    { ModelType::AlarmsWarn, { "Адрес", "Описание", "Подсветка" } },                                     //
    { ModelType::AlarmsInfo, { "Адрес", "Описание" } },                                                  //
    { ModelType::Journals, { "XML", "Описание" } },                                                      //
    { ModelType::WorkJours, { "Адрес", "Описание" } },                                                   //
    { ModelType::MeasJours, { "Индекс", "Заголовок", "Тип", "Видимость" } },                             //
    { ModelType::Modbus, { "ID сигнала", "Тип регистра", "Возвращаемый тип", "Описание" } },             //
    { ModelType::Protocom, { "Блок", "ID сигнала" } },                                                   //
    { ModelType::IEC60870, { "ID сигнала", "Тип сигнала", "Тип передачи", "Группа" } },                  //
    { ModelType::Config, { "ID виджета", "Значение по умолчанию", "Изм. количество", "Видимость" } },    //
    { ModelType::Hidden, { "Название", "Префикс", "Флаг", "Задний фон" } },                              //
    { ModelType::HiddenTab, { "Индекс", "Название", "Виджет", "Тип", "Данные", "Адрес", "Видимость" } }, //
};

/// \brief Base XML model class ctor
XmlModel::XmlModel(int rows, int cols, ModelType type, QObject *parent) : BaseEditorModel(rows, cols, type, parent)
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

/// \brief Slot for inserting a new item in the model.
void XmlModel::create(const QStringList &saved, int *row)
{
    BaseEditorModel::create(saved, row);
    emit modelChanged();
}

/// \brief Slot for updating an item's data in the model.
void XmlModel::update(const QStringList &saved, const int row)
{
    BaseEditorModel::update(saved, row);
    emit modelChanged();
}

/// \brief Slot for deleting an exisiting item in the model.
void XmlModel::remove(const int row)
{
    BaseEditorModel::remove(row);
    emit modelChanged();
}

/*! \brief Parses given XML DOM node in current XML model.
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
void XmlModel::parseTag(QDomNode &node, const QString &tagName, int row, int col, const QString &defValue, bool isInt)
{
    auto namedNode = node.firstChildElement(tagName);
    auto tagIndex = index(row, col);
    if (!namedNode.isNull())
    {
        auto tag = namedNode.firstChild().toText().data();
        if (isInt)
        {
            auto status = true;
            auto intValue = tag.toInt(&status);
            if (status)
                setData(tagIndex, intValue);
            else
                setData(tagIndex, tag);
        }
        else
            setData(tagIndex, tag);
    }
    else
    {
        if (!defValue.isEmpty())
        {
            if (isInt)
            {
                auto status = true;
                auto intValue = defValue.toInt(&status);
                if (status)
                    setData(tagIndex, intValue);
                else
                    setData(tagIndex, defValue);
            }
            else
                setData(tagIndex, defValue);
        }
    }
}

/*! \brief Parses given attribute from given XML DOM node in XML model at given index.
 *  \details Frequently called by implementations of parseNode virtual function.
 *  \see parseTag, parseNode
 */
void XmlModel::parseAttribute(QDomNode &node, const QString &attrName, int row, int col, const QString &defValue)
{
    auto attr = node.toElement().attribute(attrName, defValue);
    if (!attr.isNull())
    {
        auto attrIndex = index(row, col);
        setData(attrIndex, attr);
    }
}
