#ifndef CONFIGSTORAGE_H
#define CONFIGSTORAGE_H

#include "../../gen/singleton.h"
#include "../../s2/valuemap.h"
#include "../../widgets/delegate_common.h"
#include "../modulesettings.h"

#include <QObject>

class ConfigStorage : public QObject, public Singleton<ConfigStorage>
{
    Q_OBJECT
private:
    DataTypes::ValueMap mS2Map;
    config::widgetMap mWidgetMap;
    categoryMap mCategoryMap; ///< скорее всего, не нужно, потому что есть ModuleSettings::mTabs
    std::unique_ptr<ModuleSettings> mSettings;
    bool isS2Parsed;

public:
    explicit ConfigStorage(QObject *parent = nullptr);
    void initSettings(const Modules::StartupInfoBlock &startupInfo);
    const bool getS2Status() const;
    void setS2Status(const bool status);

    auto &getStartupInfoBlock()
    {
        return mSettings->getStartupInfoBlock();
    }

public slots:
    void typeDataReceive(const quint16 &id, const ctti::unnamed_type_id_t &type);
    void widgetDataReceive(const quint16 &id, const config::itemVariant &widget);
};

#endif // CONFIGSTORAGE_H
