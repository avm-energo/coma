#ifndef XMLCONFIGPARSER_H
#define XMLCONFIGPARSER_H

#include <QMap>
#include <QString>

class XmlConfigParser
{
public:
    static void ParseS2ConfigToMap(QMap<QString, quint16> &map);
};

#endif // XMLCONFIGPARSER_H
