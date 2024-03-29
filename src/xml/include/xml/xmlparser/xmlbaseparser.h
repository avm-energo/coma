#pragma once

#include <QObject>
#include <QtXml>
#include <ctti/type_id.hpp>
#include <xml/xmltags.h>

namespace Xml
{
/// \brief Base classe for XML parser classes.
/// \see ModuleParser, S2Parser.
class BaseParser : public QObject
{
    Q_OBJECT
public:
    explicit BaseParser(QObject *parent = nullptr);

signals:
    void parseError(const QString &errMsg);

protected:
    /// \brief Проверка, существует ли указанный файл.
    bool isFileExist(const QString &filename);
    /// \brief Returns QDomDocument for a file with the specified filename from user's local folder.
    QDomDocument getFileContent(const QString &filename);

    /// \brief Парсит ноду с дочерними элементами в QStringList.
    const QStringList parseArray(const QDomNode &node, const QString &tag) const;
    /// \brief Парсит ноду <string-array> в QStringList.
    const QStringList parseStringArray(const QDomNode &node) const;
    /// \brief Парсинг значений указанного типа в из ноды tag в QList.
    template <typename T> const QList<T> parseNumArray(const QDomNode &node, const QString &tag) const;

    /// \brief Возвращает содержимое ноды tagName в QString.
    const QString parseString(const QDomNode &node, const QString &tagName) const;
    /// \brief Callback для вызова функции functor, если у указанного
    /// узла parent существует дочерний узел с именем tagName.
    void callIfNodeExist(const QDomNode &parent, const QString &tagName, //
        const std::function<void(const QDomNode &node)> &functor);
    /// \brief Callback для вызова функции functor, для каждого
    /// дочернего узла node указанного узла parent.
    void callForEachChild(const QDomNode &parent, const std::function<void(const QDomNode &node)> &functor);
    /// \brief Вызов callForEachChild внутри callIfNodeExist для функции functor.
    /// \see callIfNodeExist, callForEachChild.
    void parseNode(const QDomNode &parent, const QString &tagName, //
        const std::function<void(const QDomNode &node)> &functor);

    /// \brief Парсинг содержимого строки в число.
    template <typename T> T parseNumString(const QString &numStr, bool &state) const;
    /// \brief Парсинг содержимого узла в число указанного типа.
    template <typename T> T parseNum(const QDomElement &numNode) const;
    /// \brief Нахождение узла с указанным именем и парсинг его содержимого, возврат числа.
    template <typename T> T parseNumFromNode(const QDomNode &node, const QString &tagName) const;
};

// Template specializations

/// \brief Template specialization for converting QString to double.
template <> double BaseParser::parseNumString(const QString &numStr, bool &state) const;
/// \brief Template specialization for converting QString to int.
template <> int BaseParser::parseNumString(const QString &numStr, bool &state) const;
/// \brief Template specialization for converting QString to uint16.
template <> quint16 BaseParser::parseNumString(const QString &numStr, bool &state) const;
/// \brief Template specialization for converting QString to uint32.
template <> quint32 BaseParser::parseNumString(const QString &numStr, bool &state) const;
/// \brief Template specialization for converting QString to uint64.
template <> quint64 BaseParser::parseNumString(const QString &numStr, bool &state) const;

// Template definitions
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

template <typename T> T BaseParser::parseNumFromNode(const QDomNode &node, const QString &tagName) const
{
    auto numNode = node.firstChildElement(tagName);
    if (!numNode.isNull())
        return parseNum<T>(numNode);
    else
        return T(0);
}

}
