#ifndef MODULEXMLPARSER_H
#define MODULEXMLPARSER_H

#include "../module/modulesettings.h"
#include "basexmlparser.h"

class ModuleXmlParser : public BaseXmlParser
{
    Q_OBJECT
private:
    bool isBase;

public:
    explicit ModuleXmlParser(QObject *parent = nullptr);
    void parse(const QDomNode &content, const quint16 &typeB, const quint16 &typeM);

signals:
    void startNewConfig();
    void signalDataSending(const quint32 &id, const quint64 &addr, const quint16 &count);
    void tabDataSending(const quint32 &id, const QString &name);
    void sectionDataSending(const ModuleTypes::SGMap &sgmap, const QString &secHead);
    void alarmDataSending(const bool &isBase, const Modules::AlarmType &aType, //
        const quint32 &addr, const QString &desc);
    void jourDataSending(const Modules::JournalType &jType, const quint32 &addr, const QString &desc);
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
    void parseAlarm(const QDomNode &alarmNode, const Modules::AlarmType &aType);
    void parseJournals(const QDomNode &joursNode);
    void parseJournal(const QDomNode &jourNode, const Modules::JournalType &jType);
    void parseModbus(const QDomNode &modbusNode);
    void parseProtocom(const QDomNode &protocomNode);
    void parseIec(const QDomNode &iecNode);
    void parseConfig(const QDomNode &configNode);
};

#endif // MODULEXMLPARSER_H
