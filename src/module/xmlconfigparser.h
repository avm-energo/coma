#ifndef XMLCONFIGPARSER_H
#define XMLCONFIGPARSER_H

#include <QMap>

class XmlConfigParser
{
public:
    static QMap<QString, quint16> NameIdMap;

    static void ParseS2ConfigToMap();
};

#endif // XMLCONFIGPARSER_H
