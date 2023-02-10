#pragma once

#include <QMap>
#include <QString>

class XmlConfigParser
{
public:
    static void ParseS2ConfigToMap(QMap<QString, quint16> &map);
};
