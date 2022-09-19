#include "basexmlparser.h"

BaseXmlParser::BaseXmlParser(QObject *parent) : QObject(parent)
{
}

/// \brief Парсим ноду <string-array> в QStringList.
const QStringList BaseXmlParser::parseStringArray(const QDomNode &node) const
{
    QStringList retList = {};
    auto strArrNode = node.firstChildElement(tags::str_array);
    if (!strArrNode.isNull() && strArrNode.hasChildNodes())
    {
        const auto &nodes = strArrNode.childNodes();
        if (!nodes.isEmpty())
        {
            const auto size = nodes.count();
            for (auto i = 0; i < size; i++)
            {
                auto strElem = nodes.item(i).toElement();
                if (!strElem.isNull())
                {
                    auto strItem = strElem.text();
                    retList.push_back(strItem);
                }
            }
        }
    }
    return retList;
}

/// \brief Возвращаем содержимое ноды tagName в QString.
const QString BaseXmlParser::parseString(const QDomNode &node, const QString &tagName) const
{
    auto textNode = node.firstChildElement(tagName);
    if (!textNode.isNull())
        return textNode.text();
    else
        return "";
}

void BaseXmlParser::parseNode(const QDomNode &parent, const QString &tagName, //
    const std::function<void(const QDomNode &node)> &functor)
{
    callIfNodeExist(parent, tagName,             //
        [this, &functor](const QDomNode &node) { //
            callForEachChild(node, functor);     //
        });
}

/// \brief Callback для вызова функции functor, если у указанного
/// узла parent существует дочерний узел с именем tagName.
void BaseXmlParser::callIfNodeExist(const QDomNode &parent, const QString &tagName, //
    const std::function<void(const QDomNode &node)> &functor)
{
    auto node = parent.firstChildElement(tagName);
    if (!node.isNull())
        functor(node);
}

/// \brief Callback для вызова функции functor, для каждого
/// дочернего узла node указанного узла parent.
void BaseXmlParser::callForEachChild(const QDomNode &parent, const std::function<void(const QDomNode &node)> &functor)
{
    auto child = parent.firstChild();
    while (!child.isNull())
    {
        if (!child.isComment() && child.isElement())
            functor(child);
        child = child.nextSibling();
    }
}

// Template specialization for converting QString to double
template <> double BaseXmlParser::parseNumString(const QString &numStr, bool &state)
{
    return numStr.toDouble(&state);
}

// Template specialization for converting QString to int
template <> int BaseXmlParser::parseNumString(const QString &numStr, bool &state)
{
    return numStr.toInt(&state);
}

// Template specialization for converting QString to uint16
template <> quint16 BaseXmlParser::parseNumString(const QString &numStr, bool &state)
{
    return numStr.toUShort(&state);
}

// Template specialization for converting QString to uint32
template <> quint32 BaseXmlParser::parseNumString(const QString &numStr, bool &state)
{
    return numStr.toUInt(&state);
}

// Template specialization for converting QString to uint64
template <> quint64 BaseXmlParser::parseNumString(const QString &numStr, bool &state)
{
    return numStr.toULongLong(&state);
}
