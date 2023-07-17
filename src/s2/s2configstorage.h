#pragma once

#include "../widgets/delegate_common.h"

#include <QObject>
#include <gen/singleton.h>

namespace S2
{

/// \brief Перечисление для статуса парсинга s2files.xml
enum class ParseStatus : bool
{
    NotYetParsed = false, ///< Пока не парсили...
    AlreadyParsed = true  ///< Уже спарсили :)
};

/// TODO: отдельная структура для детализированной информации о виджете?
using WidgetDetail = std::pair<bool, quint32>;

class ConfigStorage : public QObject, public Singleton<ConfigStorage>
{
    Q_OBJECT
private:
    std::map<QString, quint32> idByName;
    std::map<quint32, ctti::unnamed_type_id_t> typeById;
    config::widgetMap widgetMap;
    std::map<quint32, QString> configTabs;
    std::map<quint32, WidgetDetail> widgetDetailMap;
    ParseStatus status;

public:
    explicit ConfigStorage(token token, QObject *parent = nullptr);

    ParseStatus getParseStatus() const;
    void setParseStatus(const ParseStatus pStatus);

    const std::map<QString, quint32> &getIdByNameMap() const;
    const std::map<quint32, ctti::unnamed_type_id_t> &getTypeByIdMap() const;
    const config::widgetMap &getWidgetMap() const;
    const std::map<quint32, QString> &getConfigTabs() const;
    const std::map<quint32, WidgetDetail> &getWidgetDetailMap() const;

    void clearDetailData() noexcept;

public slots:
    void nameDataReceive(const quint32 id, const QString &name);
    void typeDataReceive(const quint32 id, const std::uint64_t typeId);
    void widgetDataReceive(const quint32 id, const config::itemVariant &widget);
    void configTabDataReceive(const quint32 id, const QString &tabName);
    void widgetDetailsReceive(const quint32 id, const bool visib, const quint16 count);
};

}

using S2ConfigStorage = S2::ConfigStorage;
