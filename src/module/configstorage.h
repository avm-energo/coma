#ifndef CONFIGSTORAGE_H
#define CONFIGSTORAGE_H

#include "../s2/s2datafactory.h"
#include "../widgets/delegate_common.h"
#include "board.h"
#include "modulesettings.h"

#include <QObject>
#include <gen/singleton.h>

class ConfigStorage : public QObject, public Singleton<ConfigStorage>
{
    Q_OBJECT
private:
    // config::widgetMap widgetMap;
    // ModuleTypes::TabsMap s2tabs;
    std::unique_ptr<ModuleSettings> mSettings;
    // S2::DataFactory s2factory;

public:
    explicit ConfigStorage(token, QObject *parent = nullptr);
    const ModuleSettings &getModuleSettings() const;
    void clearModuleSettings();

public slots:
    // Module data slots
    // [[deprecated]] void startNewConfig();
    void signalDataReceive(const quint32 id, const quint32 addr, //
        const quint16 count, const ModuleTypes::SignalType sigType);
    void tabDataReceive(const quint32 id, const QString &name);
    void sectionDataReceive(const ModuleTypes::SGMap &sgmap, const QString &secHead);
    void alarmDataReceive(const Modules::AlarmType aType, const quint32 addr, //
        const QString &desc, const QList<quint32> &highlights);
    void workJourDataReceive(const quint32 id, const QString &desc);
    void measJourDataReceive(const quint32 index, const QString &header, //
        const ModuleTypes::BinaryType type, bool visib);
    // [[deprecated]] void configDataReceive(
    //    const quint16 id, const QString &defVal, const bool visib, const quint16 count);
    void protocolDescriptionReceived(const parseXChangeStruct &str);
};

#endif // CONFIGSTORAGE_H
