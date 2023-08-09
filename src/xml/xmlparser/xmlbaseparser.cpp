#include "xmlbaseparser.h"

#include <gen/error.h>
#include <gen/stdfunc.h>

Xml::BaseParser::BaseParser(QObject *parent) : QObject(parent)
{
}

/// \brief Returns QDomDocument for a file with the specified filename from user's local folder.
QDomDocument Xml::BaseParser::getFileContent(const QString &filename)
{
    QDomDocument doc;
    QDir dir(StdFunc::GetSystemHomeDir());
    QFile file(dir.filePath(filename));
    if (file.exists())
    {
        if (file.open(QIODevice::ReadOnly))
        {
            QString errMsg = "";
            auto line = 0, column = 0;
            if (!doc.setContent(&file, &errMsg, &line, &column))
            {
                errMsg = "File: " + file.fileName() + " Error: " + errMsg + //
                    " Line: " + QString::number(line) + " Column: " + QString::number(column);
                emit parseError(errMsg);
            }
            else if (doc.isNull())
                emit parseError("File is empty: " + file.fileName());
            file.close();
        }
        else
            emit parseError("File open error: " + file.fileName());
    }
    else
        emit parseError("File not found: " + file.fileName());

    return doc;
}

/// \brief Парсим ноду с дочерними элементами в QStringList.
const QStringList Xml::BaseParser::parseArray(const QDomNode &node, const QString &tag) const
{
    QStringList retList = {};
    auto strArrNode = node.firstChildElement(tag);
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

/// \brief Парсим ноду <string-array> в QStringList.
const QStringList Xml::BaseParser::parseStringArray(const QDomNode &node) const
{
    return parseArray(node, tags::str_array);
}

/// \brief Возвращаем содержимое ноды tagName в QString.
const QString Xml::BaseParser::parseString(const QDomNode &node, const QString &tagName) const
{
    auto textNode = node.firstChildElement(tagName);
    if (!textNode.isNull())
        return textNode.text();
    else
        return "";
}

/// \brief Вызов callForEachChild внутри callIfNodeExist для функции functor.
/// \see callIfNodeExist, callForEachChild.
void Xml::BaseParser::parseNode(const QDomNode &parent, const QString &tagName, //
    const std::function<void(const QDomNode &node)> &functor)
{
    callIfNodeExist(parent, tagName,             //
        [this, &functor](const QDomNode &node) { //
            callForEachChild(node, functor);     //
        });
}

/// \brief Callback для вызова функции functor, если у указанного
/// узла parent существует дочерний узел с именем tagName.
void Xml::BaseParser::callIfNodeExist(const QDomNode &parent, const QString &tagName, //
    const std::function<void(const QDomNode &node)> &functor)
{
    auto node = parent.firstChildElement(tagName);
    if (!node.isNull())
        functor(node);
}

/// \brief Callback для вызова функции functor, для каждого
/// дочернего узла node указанного узла parent.
void Xml::BaseParser::callForEachChild(const QDomNode &parent, const std::function<void(const QDomNode &node)> &functor)
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
template <> double Xml::BaseParser::parseNumString(const QString &numStr, bool &state) const
{
    return numStr.toDouble(&state);
}

// Template specialization for converting QString to int
template <> int Xml::BaseParser::parseNumString(const QString &numStr, bool &state) const
{
    return numStr.toInt(&state);
}

// Template specialization for converting QString to uint16
template <> quint16 Xml::BaseParser::parseNumString(const QString &numStr, bool &state) const
{
    return numStr.toUShort(&state);
}

// Template specialization for converting QString to uint32
template <> quint32 Xml::BaseParser::parseNumString(const QString &numStr, bool &state) const
{
    return numStr.toUInt(&state);
}

// Template specialization for converting QString to uint64
template <> quint64 Xml::BaseParser::parseNumString(const QString &numStr, bool &state) const
{
    return numStr.toULongLong(&state);
}
