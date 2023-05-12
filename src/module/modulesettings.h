#pragma once

#include "../s2/datarecv.h"
#include "modules.h"

#include <QHash>
#include <gen/datatypes.h>

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
enum ViewType : quint8
{
    Float = 0,
    Bitset = 1
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
    QString name;           ///< атрибут "header"
    QList<MWidget> widgets; ///< узлы <mwidget>
};

using SGMap = QMultiMap<quint32, SGroup>; ///< quint32 - id вкладки, SGroup - группы для вкладки.

/// \brief Структура для хранения информации узла <section> из XML.
struct Section
{
    QString name; ///< атрибут "header"
    SGMap sgMap;  ///< узлы <sgroup>
};

/// \brief Перечисление для хранение типа, читаемого из бинарного файла.
enum BinaryType : quint8
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

/// \brief Структура для хранения информации о протоколах protocom, modbus и iec104.
struct InterfaceSettings
{
    QVariant settings;
};

using SignalMap = QHash<quint32, Signal>;        ///< Хранит узлы <signal> секции <signals>.
using TabsMap = QHash<quint32, QString>;         ///< Хранит узлы <tab> секции <section-tabs>.
using ConfigList = QList<DataTypes::RecordPair>; ///< Хранит узлы <record> секции <config>.
using ConfigMap = QHash<quint32, ConfigList>; ///< Хранение конфигурации для базы и мезонина в разных списках.
using HighlightMap = QMultiMap<quint32, quint32>; ///< Для подсветки элементов.
using SectionList = QList<Section>;               ///< Хранит узлы <section> секции <sections>.
using AlarmKey = Modules::AlarmType;              ///< Modules::AlarmType - тип сигнализации.
using AlarmValue = QMap<quint32, QString>; ///< quint32 - адрес сигнализации, QString - нода <desc> (описание).
using AlarmMap = QHash<AlarmKey, AlarmValue>; ///< Хранит узлы <item> секции <alarms>.
using WorkJourMap = QMap<quint32, QString>;   ///< Хранит узлы <item> секции <work> из <journals>.
using MeasJourList = QList<MeasJournal>;      ///< Хранит узлы <item> секции <meas> из <journals>.
using DetailCountMap
    = QHash<quint16, quint16>; ///< Хранит количество элементов для конфигурационных параметров, имеющих одинаковые id.

}

/// \brief Class for storing module settings.
class ModuleSettings
{
public:
    ModuleSettings();
    void clear();
    void startNewConfig();
    void appendToCurrentConfig(const DataTypes::RecordPair &pair);
    void appendDetailCount(const quint16 id, const quint16 count);
    void appendSignal(const quint32 id, const ModuleTypes::Signal sig);
    void appendTab(const quint32 id, const QString &tabName);
    void appendSection(const ModuleTypes::Section &section);
    void appendAlarm(const ModuleTypes::AlarmKey &key, const quint32 &addr, const QString &desc);
    void appendHighlight(const Modules::AlarmType &type, const quint32 &key, const QList<quint32> &values);
    void appendWorkJournal(const quint32 id, const QString &desc);
    void appendMeasJournal(const quint32 index, const QString &header, //
        const ModuleTypes::BinaryType type, bool visib);
    void setInterfaceSettings(const ModuleTypes::InterfaceSettings &settings);

    const ModuleTypes::ConfigMap &getConfigMap() const;
    const ModuleTypes::ConfigList getConfigs() const;
    const ModuleTypes::DetailCountMap &getDetailConfigCount() const;
    const ModuleTypes::SignalMap &getSignals() const;
    const ModuleTypes::TabsMap &getTabs() const;
    const ModuleTypes::SectionList &getSections() const;
    const ModuleTypes::AlarmMap &getAlarms() const;
    const ModuleTypes::HighlightMap &getHighlights(const Modules::AlarmType &type) const;
    const ModuleTypes::WorkJourMap &getWorkJours() const;
    const ModuleTypes::MeasJourList &getMeasJours() const;
    const ModuleTypes::InterfaceSettings &getInterfaceSettings() const;

private:
    int curConfigIndex;
    ModuleTypes::ConfigMap mConfigs;
    ModuleTypes::DetailCountMap mCountMap;
    ModuleTypes::SignalMap mSignals;
    ModuleTypes::TabsMap mTabs;
    ModuleTypes::SectionList mSections;
    ModuleTypes::AlarmMap mAlarms;
    ModuleTypes::HighlightMap critHighlight, warnHighlight;
    ModuleTypes::WorkJourMap workJournals;
    ModuleTypes::MeasJourList measJournals;
    ModuleTypes::InterfaceSettings mIfaceSettings;
};
