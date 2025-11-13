#pragma once

#include <gen/integers.h>
#include <gen/singleton.h>
#include <s2/delegate_common.h>

#include <QObject>

namespace S2
{

/// \brief Класс для хранения XML конфигурации, необходимой для работы с данными в формате S2.
class ConfigStorage : public QObject
{
    Q_OBJECT
public:
    /// \brief Детализированная информация о виджете
    struct WidgetDetail
    {
        bool isVisible;
        quint16 count;
        quint16 order;
    };

    explicit ConfigStorage(QObject *parent = nullptr);

    /// \brief Очищает изменяемые данные, характерные для модуля.
    /// \details Такими данными являются: видимость виджета,
    /// уточнённое количество виджетов в группе.
    void clearDetailData() noexcept;

    /// \brief Возвращает std::map, где key - имя записи S2, value - ID записи S2.
    /// \details Позволяет искать данные по имени.
    const QMap<QString, u32> &getIdByNameMap() const;
    /// \brief Возвращает std::map, где key - ID записи S2,
    /// value - хэш типа хранимых данных, в записи с указанным S2 ID.
    const QMap<u32, ctti::detail::hash_t> &getTypeByIdMap() const;
    /// \brief Возвращает true, если id дебажный и false, если нет
    bool getDType(u32 id);
    /// \brief Возвращает std::map, где key - ID записи S2, value - структура,
    /// описывающая виджет для представления данных указанной записи S2.
    const config::widgetMap &getWidgetMap() const;
    /// \brief Возвращает std::map, где key - ID вкладки, value - строка с именем вкладки.
    const QMap<u32, QString> &getConfigTabs() const;
    /// \brief Возвращает std::map, где key - ID записи S2,
    /// value - уточнённые данные для отображения виджета с указанным S2 ID.
    const QMap<quint32, WidgetDetail> &getWidgetDetailMap() const;

    /// \brief Возвращает ID записи S2 по её имени.
    /// \details В случае, если данной записи не существует, функция вернёт ID равный 0.
    u32 getIdFor(const QString &name) const noexcept;

private:
    QMap<QString, u32> m_idByName;
    QMap<u32, ctti::detail::hash_t> m_typeById;
    QMap<u32, bool> m_dtypes;
    config::widgetMap m_widgetMap;
    QMap<u32, QString> m_configTabs;
    QMap<u32, WidgetDetail> m_widgetDetailMap;

public slots:
    /// \brief Слот для сохранения S2 ID по его имени.
    /// \see getIdByNameMap.
    void nameDataReceive(const quint32 id, const QString &name);
    /// \brief Слот для сохранения информации о типе данных записи S2 по его S2 ID.
    /// \see getTypeByIdMap.
    void typeDataReceive(const quint32 id, const ctti::detail::hash_t typeId);
    /// \brief Слот для сохранения информации о дебажности параметра
    /// \param typeId: Yes - дебажный, No или пустая строка - сервисный
    void dtypeDataReceive(const quint32 id, bool dtype);
    /// \brief Слот для сохранения информации о виджете по его S2 ID.
    /// \see getWidgetMap.
    void widgetDataReceive(const quint32 id, const config::itemVariant &widget);
    /// \brief Слот для сохранения имени вкладки по её ID.
    /// \see getConfigTabs.
    void configTabDataReceive(const quint32 id, const QString &tabName);
    /// \brief Слот для сохранения уточнённой информации о виджете по его S2 ID.
    /// \see getWidgetDetailMap.
    void widgetDetailsReceive(const quint32 id, const bool visib, const quint16 count, const quint16 order);
    config::itemVariant mergeWidgets(const config::itemVariant &oldWidget, const config::itemVariant &newWidget);
};

} // namespace S2

using S2ConfigStorage = S2::ConfigStorage;
