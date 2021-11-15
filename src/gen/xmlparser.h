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
class XmlParser
{
public:
    XmlParser();
    static DataTypes::Alarm parseAlarm(QDomElement domElement);
    static DataTypes::JournalDesc parseJournal(QDomElement domElement);

    static ctti::unnamed_type_id_t parseType(QDomElement domElement);
    static config::itemVariant parseWidget(QDomElement domElement);
    static DataTypes::RecordPair parseRecord(QDomElement domElement, config::widgetMap *const s2widgetMap);
    static check::itemVariant parseRecord(QDomElement domElement);
    static check::detail::Record parseRecordHelper(QDomElement domElement);
    static config::Item parseItem(QDomElement domElement, ctti::unnamed_type_id_t parentType);
    static void traverseNode(const QDomNode &node, ModuleSettings *const settings, ConfigSettings &configSettings);
    static void traverseNode(const QDomNode &node, ConfigSettings &settings);
    static void traverseNode(const QDomNode &node, categoryMap &settings);
    static void traverseNodeS2(
        const QDomNode &node, QList<DataTypes::RecordPair> &settings, config::widgetMap *widgets);
    static void traverseNodeCheck(const QDomNode &node, check::itemVector &settings);

private:
    static quint32 parseInt32(QDomElement domElement);
    static quint64 parseHexInt64(QDomElement domElement);
    static QStringList parseStringList(QDomElement domElement);
    template <typename Container> static Container parseMap(QDomElement domElement);

    static void mergeWidget(const QDomElement &domElement, config::widgetMap *const s2widgetMap, BciNumber id);

    static bool isCorrectModule(const QString &typem, const QString &typeb, quint16 m_typem, quint16 m_typeb);
    static bool isOutdated(quint32 configVersion, quint32 realVersion)
    {
        return realVersion < configVersion;
    }
};
