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
    const QStringList parseArray(const QDomNode &node, const QString &tag) const;
    const QStringList parseStringArray(const QDomNode &node) const;
    template <typename T> const QList<T> parseNumArray(const QDomNode &node, const QString &tag) const;

    const QString parseString(const QDomNode &node, const QString &tagName) const;
    void callIfNodeExist(const QDomNode &parent, const QString &tagName, //
        const std::function<void(const QDomNode &node)> &functor);
    void callForEachChild(const QDomNode &parent, const std::function<void(const QDomNode &node)> &functor);
    void parseNode(const QDomNode &parent, const QString &tagName, //
        const std::function<void(const QDomNode &node)> &functor);

    template <typename T> T parseNumString(const QString &numStr, bool &state) const;
    template <typename T> T parseNum(const QDomElement &numNode) const;
    template <typename T> T parseNumFromNode(const QDomNode &node, const QString &tagName) const;
};

// Template specializations

template <> double BaseParser::parseNumString(const QString &numStr, bool &state) const;
template <> int BaseParser::parseNumString(const QString &numStr, bool &state) const;
template <> quint16 BaseParser::parseNumString(const QString &numStr, bool &state) const;
template <> quint32 BaseParser::parseNumString(const QString &numStr, bool &state) const;
template <> quint64 BaseParser::parseNumString(const QString &numStr, bool &state) const;

// Template definitions
/// \brief Парсинг значений указанного типа в из ноды tag в QList.
template <typename T> const QList<T> Xml::BaseParser::parseNumArray(const QDomNode &node, const QString &tag) const
{
    QList<quint32> retList = {};
    bool state = false;
    for (const auto &val : parseArray(node, tag))
    {
        auto number = parseNumString<T>(val, state);
        if (state)
            retList.append(number);
        else
            retList.append(T(0));
    }
    return retList;
}

/// \brief Парсинг содержимого узла в число указанного типа.
template <typename T> T BaseParser::parseNum(const QDomElement &numNode) const
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

/// \brief Нахождение узла с указанным именем и парсинг его содержимого, возврат числа.
template <typename T> T BaseParser::parseNumFromNode(const QDomNode &node, const QString &tagName) const
{
    auto numNode = node.firstChildElement(tagName);
    if (!numNode.isNull())
        return parseNum<T>(numNode);
    else
        return T(0);
}

}
