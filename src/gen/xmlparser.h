#pragma once

#include "datatypes.h"
#include "settings.h"

#include <QDomElement>
#include <QObject>
namespace delegate
{

}
struct ModuleSettings;
struct GlobalSettings;
class XmlParserHelper;
class XmlParser
{
    friend XmlParserHelper;

public:
    XmlParser();

    static config::Item parseItem(QDomElement domElement, ctti::unnamed_type_id_t parentType);
    static void traverseNode(const QDomNode &node, ModuleSettings *const settings, ConfigSettings &configSettings);
    static void traverseNode(const QDomNode &node, ConfigSettings &settings);
    static void traverseNode(const QDomNode &node, categoryMap &settings);
    static void traverseNodeS2(
        const QDomNode &node, QList<DataTypes::RecordPair> &settings, config::widgetMap *widgets);
    static void traverseNodeCheck(const QDomNode &node, std::vector<CheckItem> &settings);

    static DataTypes::Alarm traverseNodeAlarm(const QDomNode &node);

private:
    static CheckItem traverseNodeCheck(const QDomNode &node);

    static DataTypes::Alarm parseAlarm(QDomElement domElement);
    static DataTypes::JournalDesc parseJournal(QDomElement domElement);

    static ctti::unnamed_type_id_t parseType(QDomElement domElement);
    static config::itemVariant parseWidget(QDomElement domElement);
    static DataTypes::RecordPair parseRecordConfig(QDomElement domElement, config::widgetMap *const s2widgetMap);
    static check::itemVariant parseRecordCheck(QDomElement domElement);

    static quint32 parseInt32(QDomElement domElement);
    static quint64 parseHexInt64(QDomElement domElement);
    static std::bitset<128> parseHexInt128(QDomElement domElement);
    static QStringList parseStringList(QDomElement domElement);
    template <typename T> static std::vector<T> parseVector(QDomElement domElement);
    template <typename T> static std::set<T> parseSet(QDomElement domElement);

    template <typename Container> static Container parseMap(QDomElement domElement);

    static void mergeWidget(const QDomElement &domElement, config::widgetMap *const s2widgetMap, BciNumber id);

    static bool isCorrectModule(const QString &typem, const QString &typeb, quint16 m_typem, quint16 m_typeb);
};

class XmlParserHelper
{
public:
    static check::detail::Record parseRecordCheck(QDomElement domElement);
    static check::detail::Signals parseSignals(QDomElement domElement);
};
