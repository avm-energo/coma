#pragma once

#include <QHash>
#include <gen/datatypes.h>
#include <interfaces/types/protocol_settings.h>
#include <s2/dataitem.h>

namespace Device
{

namespace ModuleTypes
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
        time64
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
                                           ///< 0x01 - базовая;
                                           ///< 0x02 - мезонин;
                                           ///< 0x04 - дополнительная.
        std::vector<HiddenWidget> widgets; ///< узлы <mwdiget>.
    };

    /// \brief Перечисление типов сигнализации
    enum AlarmType : u8
    {
        Info,
        Warning,
        Critical
    };

    using SignalMap = std::map<u32, Signal>;  ///< Хранит узлы <signal> секции <signals>.
    using TabsMap = QHash<u32, QString>;      ///< Хранит узлы <tab> секции <section-tabs>.
    using HighlightMap = QMultiMap<u32, u32>; ///< Для подсветки элементов.
    using SectionList = std::vector<Section>; ///< Хранит узлы <section> секции <sections>.
    using AlarmValue = QMap<u32, QString>; ///< u32 - адрес сигнализации, QString - узел <desc> (описание).
    using WorkJourMap = QMap<u32, QString>; ///< Хранит узлы <item> секции <work> из <journals>.
    using AlarmMap = QHash<AlarmType, AlarmValue>; ///< Хранит узлы <item> секции <alarms>.
    using MeasJourList = std::vector<MeasJournal>; ///< Хранит узлы <item> секции <meas> из <journals>.
    using HiddenSettings = std::vector<HiddenTab>; ///< Хранит узлы <tab> секции <hidden>.
    using DetailCountMap
        = QHash<u16, u16>; ///< Хранит количество элементов для конфигурационных параметров, имеющих одинаковые id.

} // namespace ModuleTypes

/// \brief Class for storing module settings.
class ModuleSettings final
{
public:
    explicit ModuleSettings() = default;
    /// \brief Очистка настроек модуля.
    void clear() noexcept;
    /// \brief Добавление нового сигнала в список сигналов.
    void appendSignal(const u32 id, const ModuleTypes::Signal sig);
    /// \brief Добавление новой вкладки в список вкладок.
    void appendTab(const u32 id, const QString &tabName);
    /// \brief Добавление раздела в список разделов.
    void appendSection(const ModuleTypes::Section &section);
    /// \brief Добавление сигнализации в список сигнализаций по заданному типу сигнализации (key).
    void appendAlarm(const ModuleTypes::AlarmType key, const u32 addr, const QString &desc);
    /// \brief Добавление подсветки указанных полей для подсветки при появлении сигнализации.
    void appendHighlight(const ModuleTypes::AlarmType type, const u32 key, const QList<u32> &values);
    /// \brief Добавление записи в список описаний рабочего журналов.
    void appendWorkJournal(const u32 id, const QString &desc);
    /// \brief Добавление записи в список о формате журнала измерений.
    void appendMeasJournal(const u32 index, const QString &header, const ModuleTypes::BinaryType type, bool visib);
    /// \brief Добавление информации о вкладках HiddenDialog.
    void appendHiddenTab(const ModuleTypes::HiddenTab &hiddenTab);

    /// \brief Constant getter for detailed count hashmap for current config list.
    [[nodiscard]] const ModuleTypes::DetailCountMap &getDetailConfigCount() const;
    /// \brief Constant getter for signals list.
    [[nodiscard]] const ModuleTypes::SignalMap &getSignals() const;
    /// \brief Constant getter for tabs list.
    [[nodiscard]] const ModuleTypes::TabsMap &getTabs() const;
    /// \brief Constant getter for sections list.
    [[nodiscard]] const ModuleTypes::SectionList &getSections() const;
    /// \brief Constant getter for alarms hashmap.
    [[nodiscard]] const ModuleTypes::AlarmMap &getAlarms() const;
    /// \brief Constant getter for alarm's highlights.
    [[nodiscard]] const ModuleTypes::HighlightMap &getHighlights(const ModuleTypes::AlarmType type) const;
    /// \brief Constant getter for work journals.
    [[nodiscard]] const ModuleTypes::WorkJourMap &getWorkJours() const;
    /// \brief Constant getter for measurement journals.
    [[nodiscard]] const ModuleTypes::MeasJourList &getMeasJours() const;
    /// \brief Constant getter for hidden dialog settings.
    [[nodiscard]] const ModuleTypes::HiddenSettings &getHiddenSettings() const;

private:
    ModuleTypes::DetailCountMap m_countMap;
    ModuleTypes::SignalMap m_signals;
    ModuleTypes::TabsMap m_tabs;
    ModuleTypes::SectionList m_sections;
    ModuleTypes::AlarmMap m_alarms;
    ModuleTypes::HighlightMap m_critHighlight, m_warnHighlight;
    ModuleTypes::WorkJourMap m_workJournals;
    ModuleTypes::MeasJourList m_measJournals;
    ModuleTypes::HiddenSettings m_hiddenSettings;
};

typedef ModuleTypes::SignalMap::value_type SigMapValue;

} // namespace Device
