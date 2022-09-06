#include "s2xmlparser.h"

#include "../gen/datatypes.h"
#include "../s2/s2helper.h"

class DoubleSpinBoxGroup;
class QLabel;
class QDoubleSpinBox;
class QCheckBox;
class QComboBox;
class QComboBoxGroup;
class QTableView;
class CheckBoxGroup;
class IPCtrl;
class QLineEdit;

S2XmlParser::S2XmlParser(QObject *parent) : BaseXmlParser(parent)
{
}

/// \brief Отбрасываем необрабатываемые типы
bool S2XmlParser::initialCheck(const QString &name)
{
    // New types from formatted s2files.xml
    // TODO: add handler for these types
    if (name.isEmpty() || name.contains("Type", Qt::CaseInsensitive) //
        || name.contains("OscHeader_Data", Qt::CaseInsensitive)      //
        || name.contains("SpectHeader_Data", Qt::CaseInsensitive)    //
        || name.contains("DataPoint_Osc85", Qt::CaseInsensitive)     //
        || name.contains("SwRepDataStruct", Qt::CaseInsensitive)     //
        || name.contains("DataPoint_Osc87", Qt::CaseInsensitive)     //
        || name.contains("DataPoint_Spect87", Qt::CaseInsensitive)   //
        || name.contains("BYTE[]", Qt::CaseInsensitive))             //
    {
        qWarning() << "Parsed unknown type";
        return false;
    }
    else
        return true;
}

/// \brief Обрабатываем особые типы
ctti::unnamed_type_id_t S2XmlParser::secondCheck(const QString &name)
{
    using namespace DataTypes;
    // 104 types
    if (name.contains("M_SP", Qt::CaseInsensitive))
        return ctti::unnamed_type_id<SinglePointWithTimeStruct>().hash();
    else if (name.contains("M_BO", Qt::CaseInsensitive))
        return ctti::unnamed_type_id<BitStringStruct>().hash();
    else if (name.contains("M_ME", Qt::CaseInsensitive))
        return ctti::unnamed_type_id<FloatStruct>().hash();
    // Widgets
    else if (name.contains("IpControl", Qt::CaseInsensitive))
        return ctti::unnamed_type_id<IPCtrl>().hash();
    else if (name.contains("ComboBoxGroup", Qt::CaseInsensitive) || //
        name.contains("Label", Qt::CaseInsensitive) ||              //
        name.contains("DoubleSpinBox", Qt::CaseInsensitive) ||      //
        name.contains("CheckBox", Qt::CaseInsensitive) ||           //
        name.contains("ComboBox", Qt::CaseInsensitive) ||           //
        name.contains("LineEdit", Qt::CaseInsensitive) ||           //
        name.contains("TableView", Qt::CaseInsensitive))            //
    {
        const auto trueName = "Q" + name;
        return ctti::id_from_name(trueName.toStdString());
    }
    else
        return 0;
}

/// \brief Возвращаем хэш типа для его идентификации
ctti::unnamed_type_id_t S2XmlParser::parseType(const QDomElement &typeNode)
{
    auto name = typeNode.text();
    name.replace(" ", "");

    if (initialCheck(name))
    {
        const auto val = secondCheck(name);
        if (val == 0)
        {
            const auto brackets = name.count('[');
            if (brackets != 0)
                name = name.replace("[", "_").replace("]", "t");
            return ctti::id_from_name(name.toStdString());
        }
        else
            return val;
    }
    else
        return 0;
}

/// \brief Парсинг тегов для структуры delegate::DoubleSpinBoxWidget и её потомков
void S2XmlParser::dSpinBoxParse(delegate::DoubleSpinBoxWidget &dsbw, const QDomElement &widgetNode)
{
    dsbw.min = parseNumFromNode<double>(widgetNode, tags::min);
    dsbw.max = parseNumFromNode<double>(widgetNode, tags::max);
    dsbw.decimals = parseNumFromNode<quint32>(widgetNode, tags::decimals);
}

/// \brief Парсинг тегов для потомков структуры delegate::Group
void S2XmlParser::groupParse(delegate::Group &group, const QDomElement &widgetNode, const QStringList &items)
{
    group.count = parseNumFromNode<quint32>(widgetNode, tags::count);
    // В оригинальном коде items для delegate::QComboBox присваивается переменной
    // model, а не items (см. функцию S2XmlParser::comboBoxParse)
    if constexpr (!is_comboBox<decltype(group)>)
        group.items = items;
}

/// \brief Парсинг тегов для структуры delegate::QComboBox и её потомков
void S2XmlParser::comboBoxParse(delegate::QComboBox &comboBox, //
    const QDomElement &widgetNode, const QStringList &items)
{
    comboBox.model = items;
    auto fieldNode = widgetNode.firstChildElement(tags::field);
    // QComboBox depends on index by default
    if (!fieldNode.isNull())
    {
        auto fieldStr = fieldNode.text();
        if (!fieldStr.isEmpty())
        {
            if (fieldStr.contains(tags::data))
                comboBox.primaryField = delegate::QComboBox::data;
            else if (fieldStr.contains(tags::bitfield))
                comboBox.primaryField = delegate::QComboBox::bitfield;
            else
                comboBox.primaryField = delegate::QComboBox::index;
        }
    }
}

/// \brief Парсинг тегов для структуры config::Item
config::Item S2XmlParser::parseItem(const QDomElement &itemNode, //
    const QString &className, const ctti::unnamed_type_id_t &type)
{
    delegate::ItemType itemType;
    if (className == "ModbusItem")
        itemType = delegate::ItemType::ModbusItem;
    else
        return { 0 };

    auto widgetGroup = parseNumFromNode<int>(itemNode, tags::group);
    switch (itemType)
    {
    case delegate::ItemType::ModbusItem:
    {
        auto parent = parseNumFromNode<quint16>(itemNode, tags::parent);
        return config::Item(type, itemType, parent, widgetGroup);
    }
    default:
        return config::Item(type, itemType, 0, widgetGroup);
    }
}

/// \brief Парсинг ноды <widget> у <record>
config::itemVariant S2XmlParser::parseWidget(const QDomElement &widgetNode)
{
    auto className = widgetNode.attribute(tags::class_);
    auto typeNode = widgetNode.firstChildElement(tags::type);
    auto type = parseType(typeNode);

    if (className.isEmpty())
    {
        auto widgetGroup = parseNumFromNode<int>(widgetNode, tags::group);
        const auto description = parseString(widgetNode, tags::string);
        const auto toolTip = parseString(widgetNode, tags::tooltip);
        const auto items = parseStringArray(widgetNode);

        switch (type.hash())
        {
        case ctti::unnamed_type_id<QDoubleSpinBox>().hash():
        {
            delegate::DoubleSpinBoxWidget widget(type, description, widgetGroup, toolTip);
            dSpinBoxParse(widget, widgetNode);
            return std::move(widget);
        }
        case ctti::unnamed_type_id<DoubleSpinBoxGroup>().hash():
        {
            delegate::DoubleSpinBoxGroup widget(type, description, widgetGroup, toolTip);
            dSpinBoxParse(widget, widgetNode);
            groupParse(widget, widgetNode, items);
            return std::move(widget);
        }
        case ctti::unnamed_type_id<CheckBoxGroup>().hash():
        {
            delegate::CheckBoxGroup widget(type, description, widgetGroup, toolTip);
            groupParse(widget, widgetNode, items);
            return std::move(widget);
        }
        case ctti::unnamed_type_id<QComboBox>().hash():
        {
            delegate::QComboBox widget(type, description, widgetGroup, toolTip);
            comboBoxParse(widget, widgetNode, items);
            return std::move(widget);
        }
        case ctti::unnamed_type_id<QComboBoxGroup>().hash():
        {
            delegate::QComboBoxGroup widget(type, description, widgetGroup, toolTip);
            groupParse(widget, widgetNode, items);
            comboBoxParse(widget, widgetNode, items);
            return std::move(widget);
        }
        default:
        {
            return std::move(delegate::Widget(type, description, widgetGroup, toolTip));
        }
        }
    }
    else
        return std::move(parseItem(widgetNode, className, type));
}

/// \brief Парсинг всех нод <record> файла s2files.xml
void S2XmlParser::parse(const QDomNode &content)
{
    auto s2filesNode = content.firstChildElement(tags::s2files);
    auto recordNode = s2filesNode.firstChildElement(tags::record);
    while (!recordNode.isNull() && (recordNode.tagName() == tags::record))
    {
        auto id = quint16(0);
        auto idNode = recordNode.firstChildElement(tags::id);
        if (!idNode.isNull())
            id = static_cast<quint16>(parseNum<uint>(idNode));

        auto typeNode = recordNode.firstChildElement(tags::type);
        if (!typeNode.isNull())
            emit typeDataSending(id, parseType(typeNode));

        auto widgetNode = recordNode.firstChildElement(tags::widget);
        if (!widgetNode.isNull())
            emit widgetDataSending(id, parseWidget(widgetNode));

        recordNode = recordNode.nextSibling().toElement();
    }
}
