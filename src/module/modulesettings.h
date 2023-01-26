#pragma once

#include "../s2/datarecv.h"
#include "modules.h"

#include <QHash>
#include <gen/datatypes.h>

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

using SGMap = QMultiHash<quint32, SGroup>; ///< quint32 - id вкладки, SGroup - группы для вкладки

/// \brief Структура для хранения информации узла <section> из XML.
struct Section
{
    QString name; ///< атрибут "header"
    SGMap sgMap;  ///< узлы <sgroup>
};

/// \brief Структура для хранения информации узла <item> в <journals>
struct Journal
{
    quint32 addr; ///< узел <addr>
    QString desc; ///< узел <desc>
};

/// \brief Структура для хранения информации о протоколах protocom, modbus и iec104
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
using AlarmMap = QHash<AlarmKey, AlarmValue>;                ///< Хранит узлы <item> секции <alarms>.
using JourMap = QHash<Modules::JournalType, QList<Journal>>; ///< Хранит узлы <item> секции <journals>.
using DetailCountMap = QHash<quint16, quint16>;              ///<
}

/// \brief Class for storing module settings.
class ModuleSettings
{
public:
    ModuleSettings();
    void clear();
    void startNewConfig();
    void appendToCurrentConfig(const DataTypes::RecordPair &pair);
    void appendDetailCount(const quint16 &id, const quint16 &count);
    void appendSignal(const quint32 &id, const ModuleTypes::Signal &sig);
    void appendTab(const quint32 &id, const QString &tabName);
    void appendSection(const ModuleTypes::Section &section);
    void appendAlarm(const ModuleTypes::AlarmKey &key, const quint32 &addr, const QString &desc);
    void appendHighlight(const Modules::AlarmType &type, const quint32 &key, const QList<quint32> &values);
    void appendJournal(const Modules::JournalType &key, const ModuleTypes::Journal &journal);
    void setInterfaceSettings(const ModuleTypes::InterfaceSettings &settings);

    const ModuleTypes::ConfigMap &getConfigMap() const;
    const ModuleTypes::ConfigList getConfigs() const;
    const ModuleTypes::DetailCountMap &getDetailConfigCount() const;
    const ModuleTypes::SignalMap &getSignals() const;
    const ModuleTypes::TabsMap &getTabs() const;
    const ModuleTypes::SectionList &getSections() const;
    const ModuleTypes::AlarmMap &getAlarms() const;
    const ModuleTypes::HighlightMap &getHighlights(const Modules::AlarmType &type) const;
    const ModuleTypes::JourMap &getJours() const;
    const ModuleTypes::InterfaceSettings &getInterfaceSettings() const;

private:
    quint32 curConfigIndex;
    ModuleTypes::ConfigMap mConfigs;
    ModuleTypes::DetailCountMap mCountMap;
    ModuleTypes::SignalMap mSignals;
    ModuleTypes::TabsMap mTabs;
    ModuleTypes::SectionList mSections;
    ModuleTypes::AlarmMap mAlarms;
    ModuleTypes::HighlightMap critHighlight, warnHighlight;
    ModuleTypes::JourMap mJournals;
    ModuleTypes::InterfaceSettings mIfaceSettings;
};