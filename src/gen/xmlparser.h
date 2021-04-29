#pragma once
#include "../config/widgetfactory.h"
#include "datatypes.h"

#include <QDomElement>
#include <QObject>

struct ModuleSettings;
struct GlobalSettings;
class XmlParser
{
public:
    XmlParser();
    static DataTypes::Alarm parseAlarm(QDomElement domElement);
    static DataTypes::JournalDesc parseJournal(QDomElement domElement);

    static ctti::unnamed_type_id_t parseType(QDomElement domElement);
    static delegate::itemVariant parseWidget(QDomElement domElement);
    static DataTypes::RecordPair parseRecord(QDomElement domElement);
    static delegate::Item parseItem(QDomElement domElement, ctti::unnamed_type_id_t parentType);
    static void traverseNode(const QDomNode &node, ModuleSettings *const settings);
    static void traverseNode(const QDomNode &node, GlobalSettings &settings);

private:
    static quint32 parseInt32(QDomElement domElement);
    static quint64 parseHexInt64(QDomElement domElement);
    static QStringList parseStringList(QDomElement domElement);
    template <typename Key, typename Value> static QMultiMap<Key, Value> parseMultiMap(QDomElement domElement);
};
