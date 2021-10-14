#pragma once

#include "../config/delegate.h"
#include "datatypes.h"

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
    static delegate::itemVariant parseWidget(QDomElement domElement);
    static DataTypes::RecordPair parseRecord(QDomElement domElement, widgetMap *const s2widgetMap);
    static delegate::Item parseItem(QDomElement domElement, ctti::unnamed_type_id_t parentType);
    static void traverseNode(const QDomNode &node, ModuleSettings *const settings, GlobalSettings &gsettings);
    static void traverseNode(const QDomNode &node, GlobalSettings &settings);

private:
    static quint32 parseInt32(QDomElement domElement);
    static quint64 parseHexInt64(QDomElement domElement);
    static QStringList parseStringList(QDomElement domElement);
    template <typename Container> static Container parseMap(QDomElement domElement);

    static void mergeWidget(const QDomElement &domElement, widgetMap *const s2widgetMap, BciNumber id);

    static bool isCorrectModule(const QString &typem, const QString &typeb, quint16 m_typem, quint16 m_typeb);
    static bool isOutdated(quint32 configVersion, quint32 realVersion)
    {
        return realVersion < configVersion;
    }
};
