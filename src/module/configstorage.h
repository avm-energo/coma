#ifndef CONFIGSTORAGE_H
#define CONFIGSTORAGE_H

#include "../widgets/delegate_common.h"
#include "board.h"
#include "modulesettings.h"

#include <QObject>
#include <gen/singleton.h>

using TypeByIdMap = std::map<int, ctti::unnamed_type_id_t>;

class ConfigStorage : public QObject, public Singleton<ConfigStorage>
{
    Q_OBJECT
private:
    TypeByIdMap s2Map;
    config::widgetMap widgetMap;
    ModuleTypes::TabsMap s2tabs;
    std::unique_ptr<ModuleSettings> mSettings;
    bool isS2Parsed;

public:
    explicit ConfigStorage(token, QObject *parent = nullptr);
    bool getS2Status() const;
    void setS2Status(const bool &status = true);
    const TypeByIdMap &getS2Map() const;
    const config::widgetMap &getWidgetMap() const;
    const ModuleTypes::TabsMap &getConfigTabs() const;
    const ModuleSettings &getModuleSettings() const;
    void clearModuleSettings();

public slots:
    // S2 data slots
    void typeDataReceive(const quint16 id, const std::uint64_t typeId);
    void widgetDataReceive(const quint16 id, const config::itemVariant &widget);
    void configTabDataReceive(const quint32 &id, const QString &tabName);

    // Module data slots
    void startNewConfig();
    void signalDataReceive(const quint32 id, const quint32 addr, //
        const quint16 count, const ModuleTypes::SignalType sigType);
    void tabDataReceive(const quint32 id, const QString &name);
    void sectionDataReceive(const ModuleTypes::SGMap &sgmap, const QString &secHead);
    void alarmDataReceive(const Modules::AlarmType aType, const quint32 addr, //
        const QString &desc, const QList<quint32> &highlights);
    void workJourDataReceive(const quint32 id, const QString &desc);
    void measJourDataReceive(const quint32 index, const QString &header, //
        const ModuleTypes::BinaryType type, bool visib);
    void configDataReceive(const quint16 id, const QString &defVal, const bool visib, const quint16 count);
    void protocolDescriptionReceived(const parseXChangeStruct &str);
};

#endif // CONFIGSTORAGE_H
