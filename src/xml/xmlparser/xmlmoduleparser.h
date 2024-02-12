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
    void signalDataSending(const u32 id, const quint64 addr, //
        const u16 count, const Xml::SignalType sigType);
    void tabDataSending(const u32 id, const QString &name);
    void sectionDataSending(const Xml::SGMap &sgmap, const QString &secHead);
    void alarmDataSending(const Xml::AlarmType type, const u32 addr, //
        const QString &desc, const QList<u32> &highlights);
    void workJourDataSending(const u32 id, const QString &desc);
    void measJourDataSending(const u32 idx, const QString &header, //
        const Xml::BinaryType type, bool visib);
    void protocolGroupSending(const Protocol::AbstractGroup &group);
    void configDataSending(const u16 id, const QString &defVal, const bool visib, const u16 count);
    void configNameSending(const QString &tabName);
    void hiddenTabDataSending(const Xml::HiddenTab &hiddenTab);

private:
    /// \brief Возвращает имя файла по типам базовой и мезонинной плат.
    QString getFileName(const quint16 typeB, const quint16 typeM);

    /// \brief Коплексная проверка на корректность подключённого устройства.
    /// \see isCorrectModuleVersion, isCorrectModuleType.
    bool isCorrectDevice(const QDomElement &moduleNode, Device::CurrentDevice *device);
    /// \brief Проверяет, совпадают ли переданные типы плат с указанными в XML-файле.
    bool isCorrectDeviceType(const QDomElement &moduleNode, Device::CurrentDevice *device);
    /// \brief Проверка корректности версии ВПО модуля.
    bool isCorrectFirmwareVersion(const QDomElement &moduleNode, Device::CurrentDevice *device);

    /// \brief Функция для парсинга типа группы сигналов.
    SignalType parseSignalType(const QDomNode &signalNode);
    /// \brief Функция для парсинга типа отображения мультивиджета.
    ViewType parseViewType(const QString &viewString);
    /// \brief Функция для парсинга типа отображаемых/отправляемых данных.
    BinaryType parseBinaryType(const QString &typeStr);

    /// \brief Функция для парсинга XML файла конфигурации устройства.
    void parseResources(const QDomElement &resourcesNode, const QStringList &nodes = {});
    /// \brief Функция для парсинга содержимого узла <resources>.
    void parseDetector(const QDomNode &node);
    /// \brief Функция для парсинга узла <signals>.
    void parseSignal(const QDomNode &sigNode);
    /// \brief Функция для парсинга узла <section-tabs>.
    void parseSTab(const QDomNode &sTabNode);
    /// \brief Функция для парсинга узла <sections>.
    void parseSection(const QDomNode &sectionNode);
    /// \brief Функция для парсинга узла <alarms>.
    void parseAlarms(const QDomNode &alarmsNode);
    /// \brief Функция для парсинга узлов <critical>, <warning> и <info> внутри <alarms>.
    void parseAlarm(const QDomNode &alarmNode, const AlarmType &type);
    /// \brief Функция для парсинга узла <journals>.
    void parseJournals(const QDomNode &joursNode);
    /// \brief Функция для парсинга узла <work> внутри <journals>.
    void parseWorkJournal(const QDomNode &jourNode);
    /// \brief Функция для парсинга узла <meas> внутри <journals>.
    void parseMeasJournal(const QDomNode &jourNode);
    /// \brief Функция для парсинга конфигурации интерфейса, по которому подключен модуль.
    void parseInterface(const QDomNode &root);
    /// \brief Функция для парсинга узла <modbus>.
    void parseModbus(const QDomNode &modbusNode);
    /// \brief Функция для парсинга узла <protocom>.
    void parseProtocom(const QDomNode &protocomNode);
    /// \brief Функция для парсинга узла <iec60870>.
    void parseIec(const QDomNode &iecNode);
    /// \brief Функция для парсинга узла <config>.
    void parseConfig(const QDomNode &configNode);
    /// \brief Функция для парсинга узлов <tab> внутри <hidden>.
    void parseHiddenTab(const QDomNode &hiddenTabNode);
};

}
