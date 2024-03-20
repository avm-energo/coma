#pragma once

#include <QHash>
#include <gen/datatypes.h>
#include <interfaces/types/protocol_settings.h>
#include <s2/dataitem.h>

namespace Device::XmlDataTypes
{

using SignalType = DataTypes::SignalTypes; ///< Перечисление для типа сигналов.

/// \brief Структура для хранения информации узла <signal> из XML.
struct Signal final
{
    u32 startAddr;
    u16 count;
    SignalType sigType;
};

/// \brief Перечисление для хранения типа отображения мультивиджета.
enum class ViewType : u8
{
    Float = 0, ///< отображение чисел с плавающей точкой
    Bitset,    ///< отображение битового поля
    LineEdit,  ///< ввод данных
    Version    ///< ввод или отображение версии
};

/// \brief Структура для хранения информации узла <mwidget> из XML.
struct MWidget final
{
    QString desc;                    ///< атрибут "desc"
    u32 startAddr;                   ///< узел <start-addr>
    u32 count = 1;                   ///< узел <count>
    QString tooltip = "";            ///< узел <toolTip>
    ViewType view = ViewType::Float; ///< атрибут "view"
    QStringList subItemList = {};    ///< узел <string-array>
};

/// \brief Структура для хранения информации узла <sgroup> из XML.
struct SGroup final
{
    QString name;                 ///< атрибут "header"
    std::vector<MWidget> widgets; ///< узлы <mwidget>
};

using SGMap = QMultiMap<u32, SGroup>; ///< u32 - id вкладки, SGroup - группы для вкладки.

/// \brief Структура для хранения информации узла <section> из XML.
struct Section final
{
    QString name; ///< атрибут "header"
    SGMap sgMap;  ///< узлы <sgroup>
};

/// \brief Перечисление для хранение типа, читаемого из бинарного файла.
enum class BinaryType : u8
{
    uint32 = 0,
    float32,
    time32,
    time64,
    string32,
    version32,
};

/// \brief Структура для хранения информации узла <item> из <journals/meas>.
struct MeasJournal final
{
    u32 index;       ///< атрибут "index"
    BinaryType type; ///< атрибут "type"
    bool visibility; ///< атрибут "visibility"
    QString header;  ///< атрибут "header"
};

/// \brief Структура для хранения информации узла <mwidget> из <hidden/tab>.
struct HiddenWidget final
{
    QString name;    ///< узел <name>
    QString title;   ///< атрибут "title"
    u32 address;     ///< узел <addr>
    u16 index;       ///< узел <index>
    BinaryType type; ///< узел <type>
    ViewType view;   ///< атрибут "view"
    bool visibility; ///< узел <visibility>
};

/// \brief Структура для хранения информации узла <tab> из <hidden>.
struct HiddenTab final
{
    QString title;                     ///< атрибут "desc"
    QString background;                ///< атрибут "background"
    QString prefix;                    ///< атрибут "prefix"
    u16 flag;                          ///< атрибут "flag", возможные значения:
                                       ///< - 0x01 - базовая;
                                       ///< - 0x02 - мезонин;
                                       ///< - 0x04 - дополнительная.
    std::vector<HiddenWidget> widgets; ///< узлы <mwdiget>.
};

/// \brief Перечисление типов сигнализации
enum AlarmType : u8
{
    Info,
    Warning,
    Critical
};

/// \brief Структура для хранения информации об элементе BSI Ext.
struct BsiExtItem
{
    u32 address;     ///< узел <addr>
    BinaryType type; ///< узел <type>
    bool visibility; ///< узел <visibility>
    QString desc;    ///< узел <desc>
};

using SignalMap = std::map<u32, Signal>;  ///< Хранит узлы <signal> секции <signals>.
using TabsMap = QHash<u32, QString>;      ///< Хранит узлы <tab> секции <section-tabs>.
using HighlightMap = QMultiMap<u32, u32>; ///< Для подсветки элементов.
using SectionList = std::vector<Section>; ///< Хранит узлы <section> секции <sections>.
using AlarmValue = QMap<u32, QString>; ///< u32 - адрес сигнализации, QString - узел <desc> (описание).
using WorkJourMap = QMap<u32, QString>;        ///< Хранит узлы <item> секции <work> из <journals>.
using AlarmMap = QHash<AlarmType, AlarmValue>; ///< Хранит узлы <item> секции <alarms>.
using MeasJourList = std::vector<MeasJournal>; ///< Хранит узлы <item> секции <meas> из <journals>.
using HiddenSettings = std::vector<HiddenTab>; ///< Хранит узлы <tab> секции <hidden>.
using DetailCountMap
    = QHash<u16, u16>; ///< Хранит количество элементов для конфигурационных параметров, имеющих одинаковые id.
using BsiExtItemList = std::vector<BsiExtItem>; ///< Хранит узлы <item> секции <bsi-ext>

/// \brief Class for storing device's settings.
class Settings final
{
public:
    explicit Settings() = default;
    /// \brief Очистка настроек модуля.
    void clear() noexcept;
    /// \brief Добавление нового сигнала в список сигналов.
    void appendSignal(const u32 id, const Signal sig);
    /// \brief Добавление новой вкладки в список вкладок.
    void appendTab(const u32 id, const QString &tabName);
    /// \brief Добавление раздела в список разделов.
    void appendSection(const Section &section);
    /// \brief Добавление сигнализации в список сигнализаций по заданному типу сигнализации (key).
    void appendAlarm(const AlarmType type, const u32 addr, const QString &desc);
    /// \brief Добавление подсветки указанных полей для подсветки при появлении сигнализации.
    void appendHighlight(const AlarmType type, const u32 key, const QList<u32> &values);
    /// \brief Добавление записи в список описаний рабочего журналов.
    void appendWorkJournal(const u32 id, const QString &desc);
    /// \brief Добавление записи в список о формате журнала измерений.
    void appendMeasJournal(const u32 index, const QString &header, const BinaryType type, bool visib);
    /// \brief Добавление информации о вкладках HiddenDialog.
    void appendHiddenTab(const HiddenTab &hiddenTab);
    /// \brief Добавление элемента BSI Ext.
    void appendBsiExtItem(const u32 addr, const BinaryType type, bool visib, const QString &desc);

    /// \brief Constant getter for detailed count hashmap for current config list.
    [[nodiscard]] const DetailCountMap &getDetailConfigCount() const;
    /// \brief Constant getter for signals list.
    [[nodiscard]] const SignalMap &getSignals() const;
    /// \brief Constant getter for tabs list.
    [[nodiscard]] const TabsMap &getTabs() const;
    /// \brief Constant getter for sections list.
    [[nodiscard]] const SectionList &getSections() const;
    /// \brief Constant getter for alarms hashmap.
    [[nodiscard]] const AlarmMap &getAlarms() const;
    /// \brief Constant getter for alarm's highlights.
    [[nodiscard]] const HighlightMap &getHighlights(const XmlDataTypes::AlarmType type) const;
    /// \brief Constant getter for work journals.
    [[nodiscard]] const WorkJourMap &getWorkJours() const;
    /// \brief Constant getter for measurement journals.
    [[nodiscard]] const MeasJourList &getMeasJours() const;
    /// \brief Constant getter for hidden dialog settings.
    [[nodiscard]] const HiddenSettings &getHiddenSettings() const;
    /// \brief Constant getter for BSI Ext settings.
    [[nodiscard]] const BsiExtItemList &getBsiExtSettings() const;

private:
    DetailCountMap m_countMap;
    SignalMap m_signals;
    TabsMap m_tabs;
    SectionList m_sections;
    AlarmMap m_alarms;
    HighlightMap m_critHighlight, m_warnHighlight;
    WorkJourMap m_workJournals;
    MeasJourList m_measJournals;
    HiddenSettings m_hiddenSettings;
    BsiExtItemList m_bsiExtSettings;
};

} // namespace Device::XmlDataTypes

namespace Device
{
using XmlSettings = Device::XmlDataTypes::Settings;
typedef XmlDataTypes::SignalMap::value_type SigMapValue;
} // namespace Device

using DeviceXmlSettings = Device::XmlSettings;
