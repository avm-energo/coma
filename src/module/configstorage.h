#ifndef CONFIGSTORAGE_H
#define CONFIGSTORAGE_H

#include "../gen/singleton.h"
#include "../s2/valuemap.h"
#include "../widgets/delegate_common.h"
#include "modulesettings.h"

#include <QObject>

class ConfigStorage : public QObject, public Singleton<ConfigStorage>
{
    Q_OBJECT
private:
    DataTypes::ValueMap mS2Map;
    config::widgetMap mWidgetMap;
    std::unique_ptr<ModuleSettings> mSettings;
    bool isS2Parsed;

public:
    explicit ConfigStorage(token, QObject *parent = nullptr);
    // S2 status functions
    const bool &getS2Status() const;
    void setS2Status(const bool &status);

    // S2 data getters
    const DataTypes::ValueMap::value_type &getS2Map() const;
    const config::widgetMap &getWidgetMap() const;

    // Module data getter
    const ModuleSettings &getModuleSettings() const;

public slots:
    // S2 data slots
    void typeDataReceive(const quint16 &id, const ctti::unnamed_type_id_t &type);
    void widgetDataReceive(const quint16 &id, const config::itemVariant &widget);

    // Module data slots
    void startNewConfig();
    void signalDataReceive(const quint32 &id, const quint32 &addr, //
        const quint16 &count, const ModuleTypes::SignalType &sigType);
    void tabDataReceive(const quint32 &id, const QString &name);
    void sectionDataReceive(const ModuleTypes::SGMap &sgmap, const QString &secHead);
    void alarmDataReceive(const bool &isBase, const Modules::AlarmType &aType, //
        const quint32 &addr, const QString &desc);
    void jourDataReceive(const Modules::JournalType &jType, const quint32 &addr, const QString &desc);
    void modbusDataReceive(const quint32 &sigId, const quint16 &regType);
    void protocomDataReceive(const quint32 &sigId, const quint32 &block);
    void iecDataReceive(const quint32 &sigId, const quint16 &transType, const quint16 &sigGroup);
    void configDataReceive(const quint32 &id, const QString &defVal, const bool &visib, const quint32 &count);
};

#endif // CONFIGSTORAGE_H
