#ifndef MODULEXMLPARSER_H
#define MODULEXMLPARSER_H

#include "../../ctti/type_id.hpp"
#include "basexmlparser.h"
#include "configstorage.h"

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

constexpr auto header = "header";
constexpr auto tab = "tab";
constexpr auto desc = "desc";
constexpr auto crit = "critical";
constexpr auto warn = "warning";
constexpr auto info = "info";

constexpr auto sig_id = "signal-id";
constexpr auto reg_type = "reg-type";
constexpr auto block = "block";
constexpr auto sig_type = "sig-type";
constexpr auto trans_type = "trans-type";
constexpr auto sig_group = "sig-group";
constexpr auto def_val = "defaultValue";
constexpr auto visibility = "visibility";
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

    void sectionDataSending(const ModuleTypes::MWidget &mw, const QString &sgTab, //
        const QString &sgHead, const QString &secHead);
    void alarmDataSending();
    void jourDataSending();

    void modbusDataSending(const quint32 &sigId, const quint16 &regType, const ctti::unnamed_type_id_t &type);
    void protocomDataSending(const quint32 &sigId, const quint32 &block);
    void iecDataSending(const quint32 &sigId, const quint16 &sigType, //
        const quint16 &transType, const quint16 &sigGroup);
    void configDataSending(const quint32 &id, const QString &defVal, const bool &visib, const quint32 &count);

private:
    bool isCorrectModule(const QDomElement &moduleNode, const quint16 &typeB, const quint16 &typeM);
    bool isCorrectModuleType(const QDomElement &moduleNode, const quint16 &typeB, const quint16 &typeM);
    bool isCorrectModuleVersion(const QDomElement &moduleNode);

    void parseResources(const QDomElement &resNode);
    void parseSignal(const QDomNode &sigNode);
    void parseSTab(const QDomNode &sTabNode);

    void parseSection(const QDomNode &sectionNode);
    void parseAlarms(const QDomNode &alarmsNode);
    void parseAlarm(const QDomNode &alarmNode);
    void parseJournal(const QDomNode &journalNode);

    void parseModbus(const QDomNode &modbusNode);
    void parseProtocom(const QDomNode &protocomNode);
    void parseIec(const QDomNode &iecNode);
    void parseConfig(const QDomNode &configNode);
};

#endif // MODULEXMLPARSER_H
