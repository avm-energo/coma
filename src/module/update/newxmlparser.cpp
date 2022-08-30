#include "newxmlparser.h"

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

NewXmlParser::NewXmlParser(QObject *parent) : QObject(parent)
{
}

// /// \brief Парсинг содержимого узла numNode в uint переменную
// uint NewXmlParser::parseUInt(const QDomElement &node)
//{
//    auto numString = node.text();
//    if (!numString.isEmpty())
//    {
//        auto state = false;
//        auto number = numString.toUInt(&state);
//        if (state)
//            return number;
//    }
//    return 0;
//}

/// \brief Отбрасываем необрабатываемые типы
bool NewXmlParser::initialCheck(const QString &name)
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
ctti::unnamed_type_id_t NewXmlParser::secondCheck(const QString &name)
{
    // 104 types
    if (name.contains("M_SP", Qt::CaseInsensitive))
        return ctti::unnamed_type_id<DataTypes::SinglePointWithTimeStruct>().hash();
    else if (name.contains("M_BO", Qt::CaseInsensitive))
        return ctti::unnamed_type_id<DataTypes::BitStringStruct>().hash();
    else if (name.contains("M_ME", Qt::CaseInsensitive))
        return ctti::unnamed_type_id<DataTypes::FloatStruct>().hash();
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
ctti::unnamed_type_id_t NewXmlParser::parseType(const QDomElement &typeNode)
{
    using namespace DataTypes;
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

/// \brief Парсим ноду <string-array> в QStringList
QStringList NewXmlParser::parseStringArray(const QDomElement &strArrNode)
{
    const auto &nodes = strArrNode.childNodes();
    QStringList retList = {};
    if (!nodes.isEmpty())
    {
        const auto size = nodes.count();
        for (auto i = 0; i < size; i++)
        {
            auto strItem = nodes.item(i++).toElement().text();
            retList.push_back(strItem);
        }
    }
    return retList;
}

void NewXmlParser::dSpinBoxParse(delegate::DoubleSpinBoxWidget &dsbw, const QDomElement &widgetNode)
{
    parseNumFromNode(widgetNode, tags::min, dsbw.min);
    parseNumFromNode(widgetNode, tags::max, dsbw.max);
    parseNumFromNode(widgetNode, tags::decimals, dsbw.decimals);
}

void NewXmlParser::groupParse(delegate::Group &group, const QDomElement &widgetNode, const QStringList &items)
{
    parseNumFromNode(widgetNode, tags::count, group.count);
    group.items = items;
}

void NewXmlParser::comboBoxParse(delegate::QComboBox &comboBox, //
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

config::itemVariant NewXmlParser::parseWidget(const QDomElement &widgetNode)
{
    auto className = widgetNode.attribute(tags::class_);
    auto typeNode = widgetNode.firstChildElement(tags::type);
    auto type = parseType(typeNode);

    if (className.isEmpty())
    {
        auto groupNode = widgetNode.firstChildElement(tags::group);
        auto widgetGroup = static_cast<delegate::WidgetGroup>(groupNode.text().toInt());
        const auto description = widgetNode.firstChildElement(tags::string).text();
        const auto toolTip = widgetNode.firstChildElement(tags::tooltip).text();

        QStringList items = {};
        auto strArrNode = widgetNode.firstChildElement(tags::str_arr);
        if (!strArrNode.isNull() && strArrNode.hasChildNodes())
            items = parseStringArray(strArrNode);

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
            widget.items.clear();
            return std::move(widget);
        }
        default:
        {
            return delegate::Widget(type, description, widgetGroup, toolTip);
        }
        }
    }
    else
    {
        // return parseItem(widgetNode, type);
    }
}

void NewXmlParser::parseS2(const QDomNode &content)
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
        {
            // settings.s2widgetMap->insert({ id, parseWidget(recordChild) });
        }
        recordNode = recordNode.nextSibling().toElement();
    }
}
