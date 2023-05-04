#include "hexparser.h"

HexParser::HexParser(QObject *parent) : QObject(parent)
{
}

bool HexParser::parseASCII(const QString &strRecord)
{
    return true;
}

void HexParser::parseFile(const QByteArray &hexFile)
{
    QString fileString = hexFile;
    auto records = fileString.split("\r\n");
    for (const auto &record : records)
    {
        parseASCII(record);
    }
}
