#include "basexmlparser.h"

BaseXmlParser::BaseXmlParser(QObject *parent) : QObject(parent)
{
}

/// \brief Парсим ноду <string-array> в QStringList
const QStringList BaseXmlParser::parseStringArray(const QDomElement &node) const
{
    QStringList retList = {};
    auto strArrNode = node.firstChildElement(tags::str_arr);
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

/// \brief Возвращаем содержимое ноды tagName в QString
const QString BaseXmlParser::parseNode(const QDomNode &node, const QString &tagName) const
{
    auto textNode = node.firstChildElement(tagName);
    if (!textNode.isNull())
        return textNode.text();
    else
        return "";
}
