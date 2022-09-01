#ifndef MODULEXMLPARSER_H
#define MODULEXMLPARSER_H

#include "basexmlparser.h"

namespace tags
{
constexpr auto module = "module";
constexpr auto resources = "resources";
constexpr auto sigs = "signals";
constexpr auto stabs = "section-tabs";
constexpr auto sections = "sections";
constexpr auto alarms = "alarms";
constexpr auto jours = "journals";
constexpr auto modbus = "modbus";
constexpr auto protocom = "protocom";
constexpr auto iec = "iec60870";
constexpr auto config = "config";
// constexpr auto module = "module";

constexpr auto mtypeb = "mtypeb";
constexpr auto mtypem = "mtypem";
constexpr auto version = "version";
constexpr auto addr = "start-addr";
// constexpr auto module = "module";
}

class ModuleXmlParser : public BaseXmlParser
{
    Q_OBJECT
private:
    bool isBase;

public:
    explicit ModuleXmlParser(QObject *parent = nullptr);
    void parse(const QDomNode &content, const quint16 &typeB, const quint16 &typeM);

signals:
    void signalDataSending(const quint32 &id, const quint64 &addr, const quint16 &count);
    void tabDataSending(const quint32 &id, const QString &name);

private:
    bool isCorrectModule(const QDomElement &moduleNode, const quint16 &typeB, const quint16 &typeM);
    bool isCorrectModuleType(const QDomElement &moduleNode, const quint16 &typeB, const quint16 &typeM);
    bool isCorrectModuleVersion(const QDomElement &moduleNode);

    void parseResources(const QDomElement &resNode);
    void parseSignals(const QDomNode &sigNode);
    void parseSTabs(const QDomNode &sTabsNode);

    void parseSections(const QDomElement &sectionsNode);
    void parseAlarms(const QDomElement &alarmsNode);
    void parseJournals(const QDomElement &journalsNode);

    void parseModbus(const QDomNode &modbusNode);
    void parseProtocom(const QDomNode &protocomNode);
    void parseIec(const QDomNode &iecNode);
    void parseConfig(const QDomNode &configNode);
};

#endif // MODULEXMLPARSER_H
