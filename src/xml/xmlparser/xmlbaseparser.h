#pragma once

#include "../../ctti/type_id.hpp"
#include "../xmltags.h"

#include <QObject>
#include <QtXml>

namespace Xml
{
/// \brief Base classe for XML parser classes.
/// \see ModuleParser, S2Parser.
class BaseParser : public QObject
{
    Q_OBJECT
public:
    explicit BaseParser(QObject *parent = nullptr);

protected:
    const QStringList parseStringArray(const QDomNode &node) const;
    const QString parseString(const QDomNode &node, const QString &tagName) const;
    void callIfNodeExist(const QDomNode &parent, const QString &tagName, //
        const std::function<void(const QDomNode &node)> &functor);
    void callForEachChild(const QDomNode &parent, const std::function<void(const QDomNode &node)> &functor);
    void parseNode(const QDomNode &parent, const QString &tagName, //
        const std::function<void(const QDomNode &node)> &functor);

    template <typename T> T parseNumString(const QString &numStr, bool &state);
    template <typename T> T parseNum(const QDomElement &numNode);
    template <typename T> T parseNumFromNode(const QDomNode &node, const QString &tagName);
};

// Template specializations

template <> double BaseParser::parseNumString(const QString &numStr, bool &state);
template <> int BaseParser::parseNumString(const QString &numStr, bool &state);
template <> quint16 BaseParser::parseNumString(const QString &numStr, bool &state);
template <> quint32 BaseParser::parseNumString(const QString &numStr, bool &state);
template <> quint64 BaseParser::parseNumString(const QString &numStr, bool &state);

// Template definitions
/// \brief Парсинг содержимого узла в число указанного типа
template <typename T> T BaseParser::parseNum(const QDomElement &numNode)
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

/// \brief Нахождение узла с указанным именем и парсинг его содержимого, возврат числа
template <typename T> T BaseParser::parseNumFromNode(const QDomNode &node, const QString &tagName)
{
    auto numNode = node.firstChildElement(tagName);
    if (!numNode.isNull())
        return parseNum<T>(numNode);
    else
        return T(0);
}

}
