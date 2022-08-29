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
        auto trueName = "Q" + name;
        return ctti::id_from_name(trueName.toStdString());
    }
    else
        return 0;
}

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
            if (brackets == 0)
            {
                return ctti::id_from_name(name.toStdString());
            }
            else
            {
                // auto trueName = name;
                auto index = name.indexOf('[');
                [[maybe_unused]] auto leftpart = name.left(index);
                [[maybe_unused]] auto rightpart = name.right(name.length() - index);
                return 0;
            }
        }
        else
            return val;
    }
    else
        return 0;

    /*
    switch (arrSize)
    {
    case 0:
        // Primitive types
        if (name.contains("BYTE", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<BYTE>().hash();
        if (name.contains("DWORD", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<DWORD>().hash();
        if (name.contains("WORD", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<WORD>().hash();
        if (name.contains("INT32", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<INT32>().hash();
        if (name.contains("FLOAT", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<float>().hash();
        if (name.contains("DoubleSpinBoxGroup", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<DoubleSpinBoxGroup>().hash();
        if (name.contains("CheckBoxGroup", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<CheckBoxGroup>().hash();

        [[fallthrough]];
    case 1:

        if (name.contains("BYTE[4]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<BYTE_4t>().hash();
        if (name.contains("BYTE[8]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<BYTE_8t>().hash();
        if (name.contains("BYTE[16]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<BYTE_16t>().hash();
        if (name.contains("BYTE[32]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<BYTE_32t>().hash();
        if (name.contains("DWORD[8]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<DWORD_8t>().hash();
        if (name.contains("DWORD[16]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<DWORD_16t>().hash();
        if (name.contains("DWORD[32]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<DWORD_32t>().hash();
        if (name.contains("WORD[4]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<WORD_4t>().hash();
        if (name.contains("WORD[8]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<WORD_8t>().hash();
        if (name.contains("WORD[16]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<WORD_16t>().hash();
        if (name.contains("WORD[32]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<WORD_32t>().hash();
        if (name.contains("FLOAT[2]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<FLOAT_2t>().hash();
        if (name.contains("FLOAT[3]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<FLOAT_3t>().hash();
        if (name.contains("FLOAT[4]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<FLOAT_4t>().hash();
        if (name.contains("FLOAT[6]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<FLOAT_6t>().hash();
        if (name.contains("FLOAT[8]", Qt::CaseInsensitive))
            return ctti::unnamed_type_id<FLOAT_8t>().hash();

        [[fallthrough]];
    default:
        // auto what = name;
        assert(false && "Unknown type");
    }
    */
}

void NewXmlParser::parseS2(const QDomNode &content)
{
    auto recordNode = content.firstChildElement(tags::record);
    while (!recordNode.isNull() && (recordNode.tagName() == tags::record))
    {
        auto id = quint16(0);
        auto idNode = recordNode.firstChildElement(tags::id);
        if (!idNode.isNull())
        {
            // id = static_cast<quint16>(XmlParser::parseInt32(recordChild));
        }

        auto typeNode = recordNode.firstChildElement(tags::type);
        if (!typeNode.isNull())
        {
            // settings.s2filesMap->insert(id, parseType(recordChild));
            emit typeDataSending(id, parseType(typeNode));
        }

        auto widgetNode = recordNode.firstChildElement(tags::widget);
        if (!widgetNode.isNull())
        {
            // settings.s2widgetMap->insert({ id, parseWidget(recordChild) });
        }
        recordNode = recordNode.nextSibling().toElement();
    }
}
