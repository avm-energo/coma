#ifndef BASEXMLPARSER_H
#define BASEXMLPARSER_H

#include <QObject>
#include <QtXml>

namespace tags
{
constexpr auto str_arr = "string-array";
constexpr auto name = "name";
constexpr auto id = "id";
constexpr auto count = "count";
}

class BaseXmlParser : public QObject
{
    Q_OBJECT
public:
    explicit BaseXmlParser(QObject *parent = nullptr);

protected:
    const QStringList parseStringArray(const QDomElement &node) const;
    const QString parseSting(const QDomNode &node, const QString &tagName) const;
    void parseNode(const QDomNode &parent, const QString &tagName, //
        const std::function<void(const QDomNode &node)> &functor);

    template <typename T> T parseNumString(const QString &numStr, bool &state);
    template <typename T> T parseNum(const QDomElement &numNode);
    template <typename T> T parseNumFromNode(const QDomNode &node, const QString &tagName);
};

// Template specializations

template <> double BaseXmlParser::parseNumString(const QString &numStr, bool &state);
template <> int BaseXmlParser::parseNumString(const QString &numStr, bool &state);
template <> quint16 BaseXmlParser::parseNumString(const QString &numStr, bool &state);
template <> quint32 BaseXmlParser::parseNumString(const QString &numStr, bool &state);
template <> quint64 BaseXmlParser::parseNumString(const QString &numStr, bool &state);

// Template definitions
/// \brief Парсинг содержимого узла в число указанного типа
template <typename T> T BaseXmlParser::parseNum(const QDomElement &numNode)
{
    auto numString = numNode.text();
    if (!numString.isEmpty())
    {
        auto state = false;
        auto number = parseNumString<T>(numString, state);
        if (state)
            return number;
        else
            qWarning() << "Parsing number failed!";
    }
    return T(0);
}

/// \brief Нахождение узла с указанным именем и парсинг его содержимого в указанное число по ссылке
template <typename T> T BaseXmlParser::parseNumFromNode(const QDomNode &node, const QString &tagName)
{
    auto numNode = node.firstChildElement(tagName);
    if (!numNode.isNull())
        return parseNum<T>(numNode);
    else
        return T(0);
}

#endif // BASEXMLPARSER_H
