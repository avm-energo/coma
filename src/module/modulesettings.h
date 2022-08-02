#ifndef MODULESETTINGS_H
#define MODULESETTINGS_H

#include "../gen/datatypes.h"
#include "../s2/datarecv.h"
#include "modules.h"

#include <QHash>

class ModuleSettings
{
public:
    // Configuration settings
    using ConfigList = QList<DataTypes::RecordPair>;
    using ConfigMap = QHash<int, ConfigList>;
    void startNewConfig();
    void appendToCurrentConfig(DataTypes::RecordPair pair);
    const ConfigMap getConfig();

    // Check settings
    struct MWidget
    {
        QString desc;
        quint32 startAdr;
        quint32 count;
        QString tooltip;
        QStringList subItemList;
    };

    struct MWidgetGroup
    {
        QString name;
        MWidget widget;
        quint32 tabNum;
    };

    using MWidgetGroupList = QList<MWidgetGroup>;
    using TabMap = QHash<int, MWidgetGroupList>;

    struct MainTab
    {
        QString name;
        TabMap widgetTabs;
    };

    using TabsMap = QHash<int, QString>;
    using CheckMap = QList<MainTab>;
    const TabsMap getTabs();
    const CheckMap getCheck();

    // Alarms settings

    using HighlightMap = QMultiMap<quint32, quint32>;
    using AlarmMap = QMap<Modules::AlarmType, DataTypes::Alarm>;
    AlarmMap alarms;

    // Journals settings
    using JourMap = QMap<Modules::JournalType, DataTypes::JournalDesc>;
    JourMap journals;

    ModuleSettings(const Modules::StartupInfoBlock &startupInfo);

private:
    const Modules::StartupInfoBlock &startupInfoBlock;

    ConfigMap m_config;
    HighlightMap m_critHighlight, m_warnHighlight;
    TabsMap m_tabs;
    CheckMap m_checkMap;

    int m_curConfigIndex;
};

#endif // MODULESETTINGS_H
