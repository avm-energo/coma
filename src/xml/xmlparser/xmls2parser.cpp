#include "xmls2parser.h"

#include "../../s2/s2helper.h"

#include <gen/datatypes.h>

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

const QHash<QString, std::uint64_t> Xml::S2Parser::nameTypeMap = {
    { "M_SP", ctti::unnamed_type_id<DataTypes::SinglePointWithTimeStruct>().hash() }, //
    { "M_BO", ctti::unnamed_type_id<DataTypes::BitStringStruct>().hash() },           //
    { "M_ME", ctti::unnamed_type_id<DataTypes::FloatStruct>().hash() },               //
    { "IpControl", ctti::unnamed_type_id<IPCtrl>().hash() },                          //
    { "CheckBoxGroup", ctti::unnamed_type_id<CheckBoxGroup>().hash() },               //
    { "DoubleSpinBoxGroup", ctti::unnamed_type_id<DoubleSpinBoxGroup>().hash() },     //
    { "QComboBoxGroup", ctti::unnamed_type_id<QComboBoxGroup>().hash() },             //
    { "QLabel", ctti::unnamed_type_id<QLabel>().hash() },                             //
    { "QDoubleSpinBox", ctti::unnamed_type_id<QDoubleSpinBox>().hash() },             //
    { "QCheckBox", ctti::unnamed_type_id<QCheckBox>().hash() },                       //
    { "QComboBox", ctti::unnamed_type_id<QComboBox>().hash() },                       //
    { "QLineEdit", ctti::unnamed_type_id<QLineEdit>().hash() },                       //
    { "QTableView", ctti::unnamed_type_id<QTableView>().hash() },                     //
    { "BYTE", ctti::unnamed_type_id<DataTypes::BYTE>().hash() },                      //
    { "BYTE[4]", ctti::unnamed_type_id<DataTypes::BYTE_4t>().hash() },                //
    { "BYTE[6]", ctti::unnamed_type_id<DataTypes::BYTE_6t>().hash() },                //
    { "BYTE[8]", ctti::unnamed_type_id<DataTypes::BYTE_8t>().hash() },                //
    { "BYTE[16]", ctti::unnamed_type_id<DataTypes::BYTE_16t>().hash() },              //
    { "BYTE[32]", ctti::unnamed_type_id<DataTypes::BYTE_32t>().hash() },              //
    { "WORD", ctti::unnamed_type_id<DataTypes::WORD>().hash() },                      //
    { "WORD[4]", ctti::unnamed_type_id<DataTypes::WORD_4t>().hash() },                //
    { "WORD[6]", ctti::unnamed_type_id<DataTypes::WORD_6t>().hash() },                //
    { "WORD[8]", ctti::unnamed_type_id<DataTypes::WORD_8t>().hash() },                //
    { "WORD[16]", ctti::unnamed_type_id<DataTypes::WORD_16t>().hash() },              //
    { "WORD[32]", ctti::unnamed_type_id<DataTypes::WORD_32t>().hash() },              //
    { "DWORD", ctti::unnamed_type_id<DataTypes::DWORD>().hash() },                    //
    { "DWORD[4]", ctti::unnamed_type_id<DataTypes::DWORD_4t>().hash() },              //
    { "DWORD[6]", ctti::unnamed_type_id<DataTypes::DWORD_6t>().hash() },              //
    { "DWORD[8]", ctti::unnamed_type_id<DataTypes::DWORD_8t>().hash() },              //
    { "DWORD[16]", ctti::unnamed_type_id<DataTypes::DWORD_16t>().hash() },            //
    { "DWORD[32]", ctti::unnamed_type_id<DataTypes::DWORD_32t>().hash() },            //
    { "FLOAT", ctti::unnamed_type_id<DataTypes::FLOAT>().hash() },                    //
    { "FLOAT[2]", ctti::unnamed_type_id<DataTypes::FLOAT_2t>().hash() },              //
    { "FLOAT[3]", ctti::unnamed_type_id<DataTypes::FLOAT_3t>().hash() },              //
    { "FLOAT[4]", ctti::unnamed_type_id<DataTypes::FLOAT_4t>().hash() },              //
    { "FLOAT[6]", ctti::unnamed_type_id<DataTypes::FLOAT_6t>().hash() },              //
    { "FLOAT[8]", ctti::unnamed_type_id<DataTypes::FLOAT_8t>().hash() },              //
    { "INT32", ctti::unnamed_type_id<DataTypes::INT32>().hash() }                     //
}; ///< Хэш-мапа для идентификации типа в рантайме

Xml::S2Parser::S2Parser(QObject *parent) : BaseParser(parent)
{
    constexpr auto filename = "s2files.xml";
    auto document = getFileContent(filename);
    if (!document.isNull())
        content = document.firstChildElement(tags::s2files);
}

/// \brief Возвращаем хэш типа для его идентификации
uint64_t Xml::S2Parser::parseType(const QDomElement &typeNode)
{
    auto name = typeNode.text();
    name.replace(" ", "");
    const auto &typeId = nameTypeMap.value(name, 0);
    if (typeId == 0)
        qDebug() << "Parsed unknown type: " << name;
    return typeId;
}

/// \brief Парсинг всех дочерних узлов <tab> узла <config-tabs>.
void Xml::S2Parser::parseConfigTab(const QDomNode &tabNode)
{
    auto id = parseNumFromNode<quint32>(tabNode, tags::id);
    auto name = parseString(tabNode, tags::name);
    emit configTabDataSending(id, name);
}

/// \brief Парсинг узла <config-tabs>.
void Xml::S2Parser::parseConfigTabs(const QDomElement &s2node)
{
    parseNode(s2node, tags::conf_tabs, [this](const QDomNode &tabNode) { parseConfigTab(tabNode); });
}

/// \brief Парсинг тегов для структуры delegate::DoubleSpinBoxWidget и её потомков
void Xml::S2Parser::dSpinBoxParse(delegate::DoubleSpinBoxWidget &dsbw, const QDomElement &widgetNode)
{
    dsbw.min = parseNumFromNode<double>(widgetNode, tags::min);
    dsbw.max = parseNumFromNode<double>(widgetNode, tags::max);
    dsbw.decimals = parseNumFromNode<quint32>(widgetNode, tags::decimals);
}

/// \brief Парсинг тегов для потомков структуры delegate::Group
void Xml::S2Parser::groupParse(delegate::Group &group, const QDomElement &widgetNode, const QStringList &items)
{
    group.count = parseNumFromNode<quint32>(widgetNode, tags::count);
    // В оригинальном коде items для delegate::QComboBox присваивается переменной
    // model, а не items (см. функцию S2XmlParser::comboBoxParse)
    if constexpr (!is_comboBox<decltype(group)>)
        group.items = items;
}

/// \brief Парсинг тегов для структуры delegate::QComboBox и её потомков
void Xml::S2Parser::comboBoxParse(delegate::QComboBox &comboBox, //
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
config::Item Xml::S2Parser::parseItem(const QDomElement &itemNode, //
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
config::itemVariant Xml::S2Parser::parseWidget(const QDomElement &widgetNode)
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

        switch (type)
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
void Xml::S2Parser::parse()
{
    if (!content.isNull())
    {
        parseConfigTabs(content);
        auto recordNode = content.firstChildElement(tags::record);
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
}
