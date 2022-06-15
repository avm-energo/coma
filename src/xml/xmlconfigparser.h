#ifndef XMLCONFIGPARSER_H
#define XMLCONFIGPARSER_H

#include <QMap>
#include <QString>

class XmlConfigParser
{
private:
    static QMap<QString, quint16> NameIdMap;

public:
    static void ParseS2ConfigToMap();
    static quint16 GetIdByName(QString name);
};

/*
    XmlConfigParser::ParseS2ConfigToMap();
*/
#endif // XMLCONFIGPARSER_H
