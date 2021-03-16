#pragma once
#include "datatypes.h"

#include <QDomElement>
#include <QObject>

struct ModuleSettings;

class XmlParser
{
public:
    XmlParser();
    static DataTypes::Alarm parseAlarm(QDomElement domElement);
    static DataTypes::Journal parseJournal(QDomElement domElement);
    static quint32 parseInt32(QDomElement domElement);
    static quint64 parseHexInt32(QDomElement domElement);
    static QStringList parseStringList(QDomElement domElement);
    static ctti::unnamed_type_id_t parseType(QDomElement domElement);
    static void traverseNode(const QDomNode &node, ModuleSettings *const settings);
    static void traverseNode(const QDomNode &node, S2DataTypes::valueTypeMap *const settings);
};
