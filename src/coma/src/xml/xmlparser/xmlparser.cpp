#include "xml/xmlparser/xmlparser.h"

#include <avm-widgets/basespinboxgroup.h>
#include <avm-widgets/checkboxgroup.h>
#include <avm-widgets/ipctrl.h>
#include <comawidgets/gasdensitywidget.h>
#include <common/appconfig.h>
#include <gen/floats.h>
#include <gen/xml/xmlparse.h>
#include <xml/xmltags.h>

#include <QTableView>
#include <QtDebug>

namespace Xml
{

XmlParser::XmlParser(QObject *parent) : QObject { parent } { }

const QHash<QString, ctti::detail::hash_t> Xml::XmlParser::s_nameTypeMap = {
    { "M_SP", ctti::unnamed_type_id<DataTypes::SinglePointWithTimeStruct>().hash() }, //
    { "M_BO", ctti::unnamed_type_id<DataTypes::BitStringStruct>().hash() },           //
    { "M_ME", ctti::unnamed_type_id<DataTypes::FloatStruct>().hash() },               //
    { "IpControl", ctti::unnamed_type_id<IPCtrl>().hash() },                          //
    { "CheckBoxGroup", ctti::unnamed_type_id<CheckBoxGroup>().hash() },               //
    { "DoubleSpinBoxGroup", ctti::unnamed_type_id<DoubleSpinBoxGroup>().hash() },     //
    { "ComboBoxGroup", ctti::unnamed_type_id<delegate::ComboBoxGroup>().hash() },     //
    { "QLabel", ctti::unnamed_type_id<QLabel>().hash() },                             //
    { "QDoubleSpinBox", ctti::unnamed_type_id<QDoubleSpinBox>().hash() },             //
    { "QCheckBox", ctti::unnamed_type_id<QCheckBox>().hash() },                       //
    { "QComboBox", ctti::unnamed_type_id<QComboBox>().hash() },                       //
    { "QLineEdit", ctti::unnamed_type_id<QLineEdit>().hash() },                       //
    { "QTableView", ctti::unnamed_type_id<QTableView>().hash() },                     //
    { "GasDensityWidget", ctti::unnamed_type_id<GasDensityWidget>().hash() },         //
    { "BYTE", ctti::unnamed_type_id<S2::BYTE>().hash() },                             //
    { "BYTE[3]", ctti::unnamed_type_id<S2::BYTE_3t>().hash() },                       //
    { "BYTE[4]", ctti::unnamed_type_id<S2::BYTE_4t>().hash() },                       //
    { "BYTE[6]", ctti::unnamed_type_id<S2::BYTE_6t>().hash() },                       //
    { "BYTE[8]", ctti::unnamed_type_id<S2::BYTE_8t>().hash() },                       //
    { "BYTE[16]", ctti::unnamed_type_id<S2::BYTE_16t>().hash() },                     //
    { "BYTE[32]", ctti::unnamed_type_id<S2::BYTE_32t>().hash() },                     //
    { "WORD", ctti::unnamed_type_id<S2::WORD>().hash() },                             //
    { "WORD[3]", ctti::unnamed_type_id<S2::WORD_3t>().hash() },                       //
    { "WORD[4]", ctti::unnamed_type_id<S2::WORD_4t>().hash() },                       //
    { "WORD[6]", ctti::unnamed_type_id<S2::WORD_6t>().hash() },                       //
    { "WORD[8]", ctti::unnamed_type_id<S2::WORD_8t>().hash() },                       //
    { "WORD[16]", ctti::unnamed_type_id<S2::WORD_16t>().hash() },                     //
    { "WORD[32]", ctti::unnamed_type_id<S2::WORD_32t>().hash() },                     //
    { "DWORD", ctti::unnamed_type_id<S2::DWORD>().hash() },                           //
    { "DWORD[4]", ctti::unnamed_type_id<S2::DWORD_4t>().hash() },                     //
    { "DWORD[6]", ctti::unnamed_type_id<S2::DWORD_6t>().hash() },                     //
    { "DWORD[8]", ctti::unnamed_type_id<S2::DWORD_8t>().hash() },                     //
    { "DWORD[16]", ctti::unnamed_type_id<S2::DWORD_16t>().hash() },                   //
    { "DWORD[32]", ctti::unnamed_type_id<S2::DWORD_32t>().hash() },                   //
    { "FLOAT", ctti::unnamed_type_id<S2::FLOAT>().hash() },                           //
    { "FLOAT[2]", ctti::unnamed_type_id<S2::FLOAT_2t>().hash() },                     //
    { "FLOAT[3]", ctti::unnamed_type_id<S2::FLOAT_3t>().hash() },                     //
    { "FLOAT[4]", ctti::unnamed_type_id<S2::FLOAT_4t>().hash() },                     //
    { "FLOAT[6]", ctti::unnamed_type_id<S2::FLOAT_6t>().hash() },                     //
    { "FLOAT[8]", ctti::unnamed_type_id<S2::FLOAT_8t>().hash() },                     //
    { "INT32", ctti::unnamed_type_id<S2::INT32>().hash() },                           //
    { "CONF_DENS[3]", ctti::unnamed_type_id<S2::GasDensity_3t>().hash() },            //
    { "CONFMAST", ctti::unnamed_type_id<S2::CONFMAST>().hash() },                     //
}; ///< Хэш-мапа для идентификации типа в рантайме

ctti::detail::hash_t Xml::XmlParser::parseType(const QDomElement &typeNode)
{
    auto name = typeNode.text();
    name.replace(" ", "");
    const auto &typeId = s_nameTypeMap.value(name, 0);
    if (typeId == 0)
        qDebug() << "Parsed unknown type: " << name;
    return typeId;
}

void Xml::XmlParser::parseConfigTab(const QDomNode &tabNode)
{
    auto id = XmlParse::parseNumFromNode<quint32>(tabNode, tags::id);
    auto name = XmlParse::parseString(tabNode, tags::name);
    emit configTabDataSending(id, name);
}

void Xml::XmlParser::parseRecord(const QDomNode &recordNode)
{
    auto id = quint32(0);
    auto idNode = recordNode.firstChildElement(tags::id);
    if (!idNode.isNull())
    {
        id = XmlParse::parseNum<quint32>(idNode);
        emit nameDataSending(id, XmlParse::parseString(recordNode, tags::name));
    }

    emit dtypeDataSending(id, AppConfiguration::notDenied(XmlParse::parseString(recordNode, tags::dtype)));

    auto typeNode = recordNode.firstChildElement(tags::type);
    if (!typeNode.isNull())
        emit typeDataSending(id, parseType(typeNode));
    auto widgetNode = recordNode.firstChildElement(tags::widget);
    if (!widgetNode.isNull())
        emit widgetDataSending(id, parseWidget(widgetNode));
}

void Xml::XmlParser::dSpinBoxParse(delegate::DoubleSpinBoxWidget &dsbw, const QDomElement &widgetNode)
{
    dsbw.min = XmlParse::parseNumFromNode<double>(widgetNode, tags::min, F4MAX);
    dsbw.max = XmlParse::parseNumFromNode<double>(widgetNode, tags::max, F4MAX);
    dsbw.decimals = XmlParse::parseNumFromNode<quint32>(widgetNode, tags::decimals, U32MAX);
}

void Xml::XmlParser::groupParse(delegate::Group &group, const QDomElement &widgetNode, const QStringList &items)
{
    group.count = XmlParse::parseNumFromNode<quint32>(widgetNode, tags::count, U32MAX);
    // В оригинальном коде items для delegate::QComboBox присваивается переменной
    // model, а не items (см. функцию S2XmlParser::comboBoxParse)
    if constexpr (!is_comboBox<decltype(group)>)
        group.items = items;
}

void Xml::XmlParser::comboBoxParse(delegate::ComboBox &comboBox, //
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
                comboBox.primaryField = delegate::ComboBox::data;
            else if (fieldStr.contains(tags::bitfield))
                comboBox.primaryField = delegate::ComboBox::bitfield;
            else if (fieldStr.contains(tags::index))
                comboBox.primaryField = delegate::ComboBox::index;
            else
                comboBox.primaryField = delegate::ComboBox::unknown;
        }
        else
            comboBox.primaryField = delegate::ComboBox::unknown;
    }
    else
        comboBox.primaryField = delegate::ComboBox::unknown;
}

config::Item Xml::XmlParser::parseItem(const QDomElement &itemNode, //
    const QString &className, const ctti::unnamed_type_id_t &type)
{
    delegate::ItemType itemType;
    if (className == "ModbusItem")
        itemType = delegate::ItemType::ModbusItem;
    else
        return { 0 };

    auto widgetGroup = XmlParse::parseNumFromNode<u16>(itemNode, tags::group, U16MAX);
    switch (itemType)
    {
    case delegate::ItemType::ModbusItem:
    {
        auto parent = XmlParse::parseNumFromNode<u16>(itemNode, tags::parent, U16MAX);
        return config::Item(type, itemType, parent, widgetGroup);
    }
    default:
        return config::Item(type, itemType, 0, widgetGroup);
    }
}

config::itemVariant Xml::XmlParser::parseWidget(const QDomElement &widgetNode)
{
    auto className = widgetNode.attribute(tags::class_);
    auto typeNode = widgetNode.firstChildElement(tags::type);
    auto type = parseType(typeNode);

    if (className.isEmpty())
    {
        u16 widgetGroup = XmlParse::parseNumFromNode<u16>(widgetNode, tags::group, U16MAX);
        const auto description = XmlParse::parseString(widgetNode, tags::string);
        const auto toolTip = XmlParse::parseString(widgetNode, tags::tooltip, description);
        const auto items = XmlParse::parseArray(widgetNode, tags::str_array);

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
            delegate::ComboBox widget(type, description, widgetGroup, toolTip);
            comboBoxParse(widget, widgetNode, items);
            return std::move(widget);
        }
        case ctti::unnamed_type_id<delegate::ComboBoxGroup>().hash():
        {
            delegate::ComboBoxGroup widget(type, description, widgetGroup, toolTip);
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
}
