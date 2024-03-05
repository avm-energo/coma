#pragma once

#include "modules.h"

#include <QHash>
#include <gen/datatypes.h>
#include <interfaces/types/interfacesettings.h>
#include <s2/dataitem.h>

constexpr auto baseConfigIndex = 0;

namespace ModuleTypes
{
using SignalType = DataTypes::SignalTypes; ///< Перечисление для типа сигналов.

/// \brief Структура для хранения информации узла <signal> из XML.
struct Signal
{
    quint32 startAddr;
    quint16 count;
    SignalType sigType;
};

/// \brief Перечисление для хранения типа отображения мультивиджета.
enum class ViewType : quint8
{
    Float = 0,
    Bitset
};

/// \brief Структура для хранения информации узла <mwidget> из XML.
struct MWidget
{
    QString desc;                    ///< атрибут "desc"
    quint32 startAddr;               ///< узел <start-addr>
    quint32 count = 1;               ///< узел <count>
    QString tooltip = "";            ///< узел <toolTip>
    ViewType view = ViewType::Float; ///< атрибут "view"
    QStringList subItemList = {};    ///< узел <string-array>
};

/// \brief Структура для хранения информации узла <sgroup> из XML.
struct SGroup
{
    QString name;                 ///< атрибут "header"
    std::vector<MWidget> widgets; ///< узлы <mwidget>
};

using SGMap = QMultiMap<quint32, SGroup>; ///< quint32 - id вкладки, SGroup - группы для вкладки.

/// \brief Структура для хранения информации узла <section> из XML.
struct Section
{
    QString name; ///< атрибут "header"
    SGMap sgMap;  ///< узлы <sgroup>
};

/// \brief Перечисление для хранение типа, читаемого из бинарного файла.
enum class BinaryType : quint8
{
    uint32 = 0,
    float32,
    time32,
    time64
};

/// \brief Структура для хранения информации узла <item> в <meas>.
struct MeasJournal
{
    quint32 index;   ///< атрибут "index"
    QString header;  ///< атрибут "header"
    BinaryType type; ///< атрибут "type"
    bool visibility; ///< атрибут "visibility"
};

using SignalMap = QHash<quint32, Signal>;         ///< Хранит узлы <signal> секции <signals>.
using TabsMap = QHash<quint32, QString>;          ///< Хранит узлы <tab> секции <section-tabs>.
using HighlightMap = QMultiMap<quint32, quint32>; ///< Для подсветки элементов.
using SectionList = std::vector<Section>;         ///< Хранит узлы <section> секции <sections>.
using AlarmKey = Modules::AlarmType;              ///< Modules::AlarmType - тип сигнализации.
using AlarmValue = QMap<quint32, QString>; ///< quint32 - адрес сигнализации, QString - нода <desc> (описание).
using AlarmMap = QHash<AlarmKey, AlarmValue>;  ///< Хранит узлы <item> секции <alarms>.
using WorkJourMap = QMap<quint32, QString>;    ///< Хранит узлы <item> секции <work> из <journals>.
using MeasJourList = std::vector<MeasJournal>; ///< Хранит узлы <item> секции <meas> из <journals>.
using DetailCountMap
    = QHash<quint16, quint16>; ///< Хранит количество элементов для конфигурационных параметров, имеющих одинаковые id.

}

/// \brief Class for storing module settings.
class ModuleSettings
{
public:
    ModuleSettings() = default;
    /// \brief Очистка настроек модуля.
    void clear();
    /// \brief Добавление нового сигнала в список сигналов.
    void appendSignal(const quint32 id, const ModuleTypes::Signal sig);
    /// \brief Добавление новой вкладки в список вкладок.
    void appendTab(const quint32 id, const QString &tabName);
    /// \brief Добавление раздела в список разделов.
    void appendSection(const ModuleTypes::Section &section);
    /// \brief Добавление сигнализации в список сигнализаций по заданному типу сигнализации (key).
    void appendAlarm(const ModuleTypes::AlarmKey &key, const quint32 &addr, const QString &desc);
    /// \brief Добавление подсветки указанных полей для подсветки при появлении сигнализации.
    void appendHighlight(const Modules::AlarmType &type, const quint32 &key, const QList<quint32> &values);
    /// \brief Добавление записи в список описаний рабочего журналов.
    void appendWorkJournal(const quint32 id, const QString &desc);
    /// \brief Добавление записи в список о формате журнала измерений.
    void appendMeasJournal(const quint32 index, const QString &header, //
        const ModuleTypes::BinaryType type, bool visib);

    /// \brief Constant getter for detailed count hashmap for current config list.
    const ModuleTypes::DetailCountMap &getDetailConfigCount() const;
    /// \brief Constant getter for signals list.
    const ModuleTypes::SignalMap &getSignals() const;
    /// \brief Constant getter for tabs list.
    const ModuleTypes::TabsMap &getTabs() const;
    /// \brief Constant getter for sections list.
    const ModuleTypes::SectionList &getSections() const;
    /// \brief Constant getter for alarms hashmap.
    const ModuleTypes::AlarmMap &getAlarms() const;
    /// \brief Constant getter for alarm's highlights.
    const ModuleTypes::HighlightMap &getHighlights(const Modules::AlarmType &type) const;
    /// \brief Constant getter for work journals.
    const ModuleTypes::WorkJourMap &getWorkJours() const;
    /// \brief Constant getter for measurement journals.
    const ModuleTypes::MeasJourList &getMeasJours() const;

private:
    ModuleTypes::DetailCountMap m_countMap;
    ModuleTypes::SignalMap m_signals;
    ModuleTypes::TabsMap m_tabs;
    ModuleTypes::SectionList m_sections;
    ModuleTypes::AlarmMap m_alarms;
    ModuleTypes::HighlightMap m_critHighlight, m_warnHighlight;
    ModuleTypes::WorkJourMap m_workJournals;
    ModuleTypes::MeasJourList m_measJournals;
};
