#pragma once

#include "../widgets/delegate_common.h"

#include <QObject>
#include <gen/singleton.h>

namespace S2
{

enum class ParseStatus : bool
{
    NotYetParsed = false,
    AlreadyParsed = true
};

class ConfigStorage : public QObject, public Singleton<ConfigStorage>
{
    Q_OBJECT
private:
    std::map<QString, quint16> idByName;
    std::map<quint16, ctti::unnamed_type_id_t> typeById;
    std::map<quint32, QString> configTabs;
    config::widgetMap widgetMap;
    ParseStatus status;

public:
    explicit ConfigStorage(token token, QObject *parent = nullptr);

    ParseStatus getParseStatus() const;
    void setParseStatus(const ParseStatus pStatus);

    const std::map<QString, quint16> &getIdByNameMap() const;
    const std::map<quint16, ctti::unnamed_type_id_t> &getTypeByIdMap() const;
    const std::map<quint32, QString> &getConfigTabs() const;
    const config::widgetMap &getWidgetMap() const;

public slots:
    void nameDataReceive(const quint16 id, const QString &name);
    void typeDataReceive(const quint16 id, const std::uint64_t typeId);
    void widgetDataReceive(const quint16 id, const config::itemVariant &widget);
    void configTabDataReceive(const quint32 id, const QString &tabName);
};

}

using S2ConfigStorage = S2::ConfigStorage;
