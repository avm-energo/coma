#pragma once

#include "xmlbaseparser.h"

#include <device/xml_settings.h>
#include <interfaces/types/common_types.h>

namespace Device
{
class CurrentDevice;
} // namespace Device

namespace Xml
{

using namespace ::Device::XmlDataTypes;

/// \brief Класс для парсинга XML файлов конфигурации устройств.
class ModuleParser : public BaseParser
{
    Q_OBJECT
private:
    Interface::IfaceType m_ifaceType;

public:
    explicit ModuleParser(QObject *parent = nullptr);

    /// \brief Парсинг определённых узлов XML файла конфигурации, передавая типы базовой и мезонинной плат устройства.
    void parse(const u16 typeB, const u16 typeM, const QStringList &nodes = {});
    /// \brief Парсинг всего XML файла конфигурации устройства, передавая текущее подключенное устройство.
    void parse(Device::CurrentDevice *device);

signals:
    void startNewConfig();
    void signalDataSending(const u32 id, const quint64 addr, const u16 count, const Xml::SignalType sigType);
    void tabDataSending(const u32 id, const QString &name);
    void sectionDataSending(const Xml::SGMap &sgmap, const QString &secHead);
    void alarmStateAllDataSending(const Xml::AlarmType type, const u32 index, const QString &desc);
    void alarmDataSending(const Xml::AlarmType type, const u32 addr, const QString &desc, const QList<u32> &highlights);
    void workJourDataSending(const u32 id, const QString &desc);
    void measJourDataSending(const u32 idx, const QString &header, const Xml::BinaryType type, bool visib);
    void protocolGroupSending(const Protocol::AbstractGroup &group);
    void configDataSending(const u16 id, const QString &defVal, const bool visib, const u16 count);
    void configNameSending(const QString &tabName);
    void hiddenTabDataSending(const Xml::HiddenTab &hiddenTab);
    void bsiExtItemDataSending(const u32 addr, const Xml::BinaryType type, const bool visib, const QString &desc);

private:
    /// \brief Возвращает имя файла по типам базовой и мезонинной плат.
    QString getFileName(const quint16 typeB, const quint16 typeM);

    /// \brief Коплексная проверка на корректность подключённого устройства.
    /// \see isCorrectModuleVersion, isCorrectModuleType.
    bool isCorrectDevice(const QDomElement &moduleNode, const Device::CurrentDevice *device);
    /// \brief Проверяет, совпадают ли переданные типы плат с указанными в XML-файле.
    bool isCorrectDeviceType(const QDomElement &moduleNode, const Device::CurrentDevice *device);
    /// \brief Проверка корректности версии ВПО модуля.
    bool isCorrectFirmwareVersion(const QDomElement &moduleNode, const Device::CurrentDevice *device);

    /// \brief Парсинг типа группы сигналов.
    SignalType parseSignalType(const QDomNode &signalNode);
    /// \brief Парсинг типа отображения мультивиджета.
    ViewType parseViewType(const QString &viewString);
    /// \brief Парсинг типа отображаемых/отправляемых данных.
    BinaryType parseBinaryType(const QString &typeStr);
    /// \brief Парсинг типа сигнализации в узле <state-all>.
    AlarmType parseAlarmType(const QString &typeStr);

    /// \brief Парсинг указанного XML файла для выбранных узлов.
    void parseDocument(const QString &filename, const QStringList &nodes = {});
    /// \brief Парсинг указанного XML файла для подключённого устройства.
    void parseDocument(const QStringList &filenames, const Device::CurrentDevice *device);
    /// \brief Парсинг XML файла конфигурации устройства.
    void parseResources(const QDomElement &resourcesNode, const QStringList &nodes = {});
    /// \brief Парсинг содержимого узла <resources>.
    void parseDetector(const QDomNode &node);
    /// \brief Парсинг узла <signals>.
    void parseSignal(const QDomNode &sigNode);
    /// \brief Парсинг узла <section-tabs>.
    void parseSTab(const QDomNode &sTabNode);
    /// \brief Парсинг узла <sections>.
    void parseSection(const QDomNode &sectionNode);
    /// \brief Парсинг узла <alarms>.
    void parseAlarms(const QDomNode &alarmsNode);
    /// \brief Парсинг узла <state-all> внутри <alarms>.
    void parseAlarmStateAll(const QDomNode &alarmStateAllNode);
    /// \brief Парсинг узлов <critical>, <warning> и <info> внутри <alarms>.
    void parseAlarm(const QDomNode &alarmNode, const AlarmType &type);
    /// \brief Парсинг узла <journals>.
    void parseJournals(const QDomNode &joursNode);
    /// \brief Парсинг узла <work> внутри <journals>.
    void parseWorkJournal(const QDomNode &jourNode);
    /// \brief Парсинг узла <meas> внутри <journals>.
    void parseMeasJournal(const QDomNode &jourNode);
    /// \brief Парсинг конфигурации интерфейса, по которому подключен модуль.
    void parseInterface(const QDomNode &root);
    /// \brief Парсинг узла <modbus>.
    void parseModbus(const QDomNode &modbusNode);
    /// \brief Парсинг узла <protocom>.
    void parseProtocom(const QDomNode &protocomNode);
    /// \brief Парсинг узла <iec60870>.
    void parseIec(const QDomNode &iecNode);
    /// \brief Парсинг узла <config>.
    void parseConfig(const QDomNode &configNode);
    /// \brief Парсинг узлов <tab> внутри <hidden>.
    void parseHiddenTab(const QDomNode &hiddenTabNode);
    /// \brief Парсинг узлов <item> внутри <bsi-ext>.
    void parseBsiExtItem(const QDomNode &bsiExtItemNode);
};

}
