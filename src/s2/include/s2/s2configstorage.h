#pragma once

#include <QObject>
#include <gen/singleton.h>
#include <s2/delegate_common.h>

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

/// \brief Класс для хранения XML конфигурации, необходимой для работы с данными в формате S2.
class ConfigStorage : public QObject, public Singleton<ConfigStorage>
{
    Q_OBJECT
private:
    std::map<QString, quint32> m_idByName;
    std::map<quint32, ctti::unnamed_type_id_t> m_typeById;
    config::widgetMap m_widgetMap;
    std::map<quint32, QString> m_configTabs;
    std::map<quint32, WidgetDetail> m_widgetDetailMap;
    ParseStatus m_status;

public:
    explicit ConfigStorage(token token, QObject *parent = nullptr);

    /// \brief Возвращает статус парсинга s2files.xml.
    ParseStatus getParseStatus() const;
    /// \brief Устанавливает статус парсинга s2files.xml.
    void setParseStatus(const ParseStatus pStatus);
    /// \brief Очищает изменяемые данные, характерные для модуля.
    /// \details Такими данными являются: видимость виджета,
    /// уточнённое количество виджетов в группе.
    void clearDetailData() noexcept;

    /// \brief Возвращает std::map, где key - имя записи S2, value - ID записи S2.
    /// \details Позволяет искать данные по имени.
    const std::map<QString, quint32> &getIdByNameMap() const;
    /// \brief Возвращает std::map, где key - ID записи S2,
    /// value - хэш типа хранимых данных, в записи с указанным S2 ID.
    const std::map<quint32, ctti::unnamed_type_id_t> &getTypeByIdMap() const;
    /// \brief Возвращает std::map, где key - ID записи S2, value - структура,
    /// описывающая виджет для представления данных указанной записи S2.
    const config::widgetMap &getWidgetMap() const;
    /// \brief Возвращает std::map, где key - ID вкладки, value - строка с именем вкладки.
    const std::map<quint32, QString> &getConfigTabs() const;
    /// \brief Возвращает std::map, где key - ID записи S2,
    /// value - уточнённые данные для отображения виджета с указанным S2 ID.
    const std::map<quint32, WidgetDetail> &getWidgetDetailMap() const;

    /// \brief Возвращает ID записи S2 по её имени.
    /// \details В случае, если данной записи не существует, функция вернёт ID равный 0.
    quint32 getIdFor(const QString &name) const noexcept;

public slots:
    /// \brief Слот для сохранения S2 ID по его имени.
    /// \see getIdByNameMap.
    void nameDataReceive(const quint32 id, const QString &name);
    /// \brief Слот для сохранения информации о типе данных записи S2 по его S2 ID.
    /// \see getTypeByIdMap.
    void typeDataReceive(const quint32 id, const std::uint64_t typeId);
    /// \brief Слот для сохранения информации о виджете по его S2 ID.
    /// \see getWidgetMap.
    void widgetDataReceive(const quint32 id, const config::itemVariant &widget);
    /// \brief Слот для сохранения имени вкладки по её ID.
    /// \see getConfigTabs.
    void configTabDataReceive(const quint32 id, const QString &tabName);
    /// \brief Слот для сохранения уточнённой информации о виджете по его S2 ID.
    /// \see getWidgetDetailMap.
    void widgetDetailsReceive(const quint32 id, const bool visib, const quint16 count);
};

} // namespace S2

using S2ConfigStorage = S2::ConfigStorage;
