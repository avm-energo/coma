#ifndef BASEXMLPARSER_H
#define BASEXMLPARSER_H

#include <QObject>
#include <QtXml>

namespace tags
{
constexpr auto str_arr = "string-array";
}

class BaseXmlParser : public QObject
{
    Q_OBJECT
public:
    explicit BaseXmlParser(QObject *parent = nullptr);
    // virtual void parse(const QDomNode &content) = 0;

protected:
    const QStringList parseStringArray(const QDomElement &node) const;
    const QString parseNode(const QDomNode &node, const QString &tagName) const;

    template <typename T> T parseString(const QString &numStr, bool &state);

    template <> double parseString(const QString &numStr, bool &state)
    {
        return numStr.toDouble(&state);
    }

    template <> int parseString(const QString &numStr, bool &state)
    {
        return numStr.toInt(&state);
    }

    template <> uint parseString(const QString &numStr, bool &state)
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
            dest = parseNum<T>(numNode);
    }
};

#endif // BASEXMLPARSER_H
