#ifndef NEWXMLPARSER_H
#define NEWXMLPARSER_H

#include "../../ctti/type_id.hpp"
#include "../../widgets/delegate_common.h"

#include <QObject>
#include <QtXml>
#include <type_traits>

namespace tags
{
constexpr auto s2files = "s2files";
constexpr auto record = "record";
constexpr auto id = "id";
constexpr auto type = "type";
constexpr auto name = "name";
constexpr auto widget = "widget";
constexpr auto class_ = "class";
constexpr auto group = "group";
constexpr auto str_arr = "string-array";
constexpr auto string = "string";
constexpr auto tooltip = "toolTip";
constexpr auto min = "min";
constexpr auto max = "max";
constexpr auto decimals = "decimals";
constexpr auto count = "count";
constexpr auto field = "field";
constexpr auto data = "data";
constexpr auto bitfield = "bitfield";
}

namespace detail
{
template <typename T> constexpr auto is_widget = std::is_convertible_v<T, delegate::Widget>;
template <typename T> constexpr auto is_group = std::is_convertible_v<T, delegate::Group>;
template <typename T> constexpr auto is_dSpinBox = std::is_convertible_v<T, delegate::DoubleSpinBoxWidget>;
template <typename T> constexpr auto is_comboBox = std::is_convertible_v<T, delegate::QComboBox>;
}

class NewXmlParser : public QObject
{
    Q_OBJECT
public:
    explicit NewXmlParser(QObject *parent = nullptr);
    void parseS2(const QDomNode &content);

signals:
    void typeDataSending(const quint16 &id, const ctti::unnamed_type_id_t &type);
    void widgetDataSending();

private:
    // uint parseUInt(const QDomElement &numNode);
    bool initialCheck(const QString &name);
    ctti::unnamed_type_id_t secondCheck(const QString &name);
    ctti::unnamed_type_id_t parseType(const QDomElement &typeNode);

    QStringList parseStringArray(const QDomElement &strArrNode);
    void dSpinBoxParse(delegate::DoubleSpinBoxWidget &dsbw, const QDomElement &widgetNode);
    void groupParse(delegate::Group &group, const QDomElement &widgetNode, const QStringList &items);
    void comboBoxParse(delegate::QComboBox &comboBox, const QDomElement &widgetNode, const QStringList &items);
    config::itemVariant parseWidget(const QDomElement &widgetNode);

    template <typename T> inline T parseString(const QString &numStr, bool &state);

    template <> inline double parseString(const QString &numStr, bool &state)
    {
        return numStr.toDouble(&state);
    }

    template <> inline uint parseString(const QString &numStr, bool &state)
    {
        return numStr.toUInt(&state);
    }

    template <typename T> T parseNum(const QDomElement &numNode)
    {
        auto numString = numNode.text();
        if (!numString.isEmpty())
        {
            auto state = false;
            auto number = parseString<T>(numString, state);
            if (state)
                return number;
            else
                qWarning() << "Parsing number failed!";
        }
        return T(0);
    }

    template <typename T> void parseNumFromNode(const QDomNode &node, const QString &tagName, T &dest)
    {
        auto numNode = node.firstChildElement(tagName);
        if (!numNode.isNull())
        {
            dest = parseNum<T>(numNode);
        }
    }
};

#endif // NEWXMLPARSER_H
