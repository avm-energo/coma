#pragma once

#include "../../interfaces/interfacesettings.h"
#include "../../module/board.h"
#include "../../module/modulesettings.h"
#include "xmlbaseparser.h"

namespace Xml
{
/// \brief Class for parsing modules configuration files.
class ModuleParser : public BaseParser
{
    Q_OBJECT
private:
    bool isBase;

public:
    explicit ModuleParser(QObject *parent = nullptr);
    void parse(const QDomNode &content, const quint16 &typeB, const quint16 &typeM);

signals:
    void startNewConfig();
    void signalDataSending(const quint32 &id, const quint64 &addr, //
        const quint16 &count, const ModuleTypes::SignalType &sigType);
    void tabDataSending(const quint32 &id, const QString &name);
    void sectionDataSending(const ModuleTypes::SGMap &sgmap, const QString &secHead);
    void alarmDataSending(const Modules::AlarmType &aType, const quint32 &addr, const QString &desc);
    void jourDataSending(const Modules::JournalType &jType, const quint32 &addr, const QString &desc);
    void interfaceSettingsSending(const QVariant &iSettings, const Board::InterfaceType &iType);
    void configDataSending(const quint32 &id, const QString &defVal, const bool &visib, const quint32 &count);

private:
    bool isCorrectModule(const QDomElement &moduleNode, const quint16 &typeB, const quint16 &typeM);
    bool isCorrectModuleType(const QDomElement &moduleNode, const quint16 &typeB, const quint16 &typeM);
    bool isCorrectModuleVersion(const QDomElement &moduleNode);
    ModuleTypes::SignalType parseSigType(const QDomNode &sigNode);
    ModuleTypes::ViewType parseViewType(const QDomNode &mwidgetNode);

    void parseResources(const QDomElement &resNode);
    void parseSignal(const QDomNode &sigNode);
    void parseSTab(const QDomNode &sTabNode);
    void parseSection(const QDomNode &sectionNode);
    void parseAlarms(const QDomNode &alarmsNode);
    void parseAlarm(const QDomNode &alarmNode, const Modules::AlarmType &aType);
    void parseJournals(const QDomNode &joursNode);
    void parseJournal(const QDomNode &jourNode, const Modules::JournalType &jType);
    void parseInterface(const QDomNode &root);
    void parseModbus(const QDomNode &modbusNode, InterfaceInfo<ModbusGroup> &settings);
    void parseProtocom(const QDomNode &protocomNode, InterfaceInfo<ProtocomGroup> &settings);
    void parseIec(const QDomNode &iecNode, InterfaceInfo<Iec104Group> &settings);
    void parseConfig(const QDomNode &configNode);
};

}
