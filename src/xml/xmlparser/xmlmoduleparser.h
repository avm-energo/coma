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
    QDomElement resources;

public:
    explicit ModuleParser(const quint16 typeB, const quint16 typeM, const bool check, QObject *parent = nullptr);
    void parse(const QStringList &nodes = {});

signals:
    void startNewConfig();
    void signalDataSending(const quint32 id, const quint64 addr, //
        const quint16 count, const ModuleTypes::SignalType sigType);
    void tabDataSending(const quint32 id, const QString &name);
    void sectionDataSending(const ModuleTypes::SGMap &sgmap, const QString &secHead);
    void alarmDataSending(const Modules::AlarmType aType, const quint32 addr, //
        const QString &desc, const QList<quint32> &highlights);
    void workJourDataSending(const quint32 id, const QString &desc);
    void measJourDataSending(const quint32 idx, const QString &header, //
        const ModuleTypes::BinaryType type, bool visib);
    void interfaceSettingsSending(const ProtocolDescription &iSettings, const Board::InterfaceType iType);
    void protocolGroupSending(const parseXChangeStruct &str);
    void configDataSending(const quint16 id, const QString &defVal, const bool visib, const quint16 count);
    void configNameSending(const QString &tabName);

private:
    QString getFileName(const quint16 typeB, const quint16 typeM);
    bool isCorrectModule(const QDomElement &moduleNode, const quint16 &typeB, const quint16 &typeM);
    bool isCorrectModuleType(const QDomElement &moduleNode, const quint16 &typeB, const quint16 &typeM);
    bool isCorrectModuleVersion(const QDomElement &moduleNode);
    ModuleTypes::SignalType parseSigType(const QDomNode &sigNode);
    ModuleTypes::ViewType parseViewType(const QDomNode &mwidgetNode);

    void parseDetector(const QDomNode &node);
    void parseSignal(const QDomNode &sigNode);
    void parseSTab(const QDomNode &sTabNode);
    void parseSection(const QDomNode &sectionNode);
    void parseAlarms(const QDomNode &alarmsNode);
    void parseAlarm(const QDomNode &alarmNode, const Modules::AlarmType &aType);
    void parseJournals(const QDomNode &joursNode);
    void parseWorkJournal(const QDomNode &jourNode);
    void parseMeasJournal(const QDomNode &jourNode);
    void parseInterface(const QDomNode &root);
    void parseModbus(const QDomNode &modbusNode);
    void parseProtocom(const QDomNode &protocomNode);
    void parseIec(const QDomNode &iecNode);
    void parseConfig(const QDomNode &configNode);
};

}
