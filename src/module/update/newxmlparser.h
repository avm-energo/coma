#ifndef NEWXMLPARSER_H
#define NEWXMLPARSER_H

#include "../../ctti/type_id.hpp"

#include <QObject>
#include <QtXml>

namespace tags
{
constexpr auto record = "record";
constexpr auto name = "name";
constexpr auto stringArray = "string-array";
constexpr auto uintSet = "uint-set";
constexpr auto string = "string";
constexpr auto color = "color";
constexpr auto unsigned32 = "quint32";
constexpr auto unsigned128 = "quint128";
constexpr auto className = "class";
constexpr auto group = "group";
constexpr auto type = "type";
constexpr auto desc = "description";
constexpr auto toolTip = "toolTip";
constexpr auto key = "key";
constexpr auto value = "value";
constexpr auto map = "map";
constexpr auto id = "id";
constexpr auto widget = "widget";
}

class NewXmlParser : public QObject
{
    Q_OBJECT
public:
    explicit NewXmlParser(QObject *parent = nullptr);
    void parseS2(const QDomNode &content);

signals:
    void typeDataSending(const quint16 &id, const ctti::unnamed_type_id_t &type);
    void bar();

private:
    bool initialCheck(const QString &name);
    ctti::unnamed_type_id_t secondCheck(const QString &name);
    ctti::unnamed_type_id_t parseType(const QDomElement &typeNode);
};

#endif // NEWXMLPARSER_H
