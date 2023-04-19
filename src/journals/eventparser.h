#pragma once

#include "../module/modulesettings.h"

#include <QTimeZone>

namespace journals
{
/// \brief Перечисление для типа события в журнале.
enum class EventType : quint8
{
    Out = 0,        ///< ушло
    In = 1,         ///< пришло
    ChangeState = 2 ///< изменение состояния
};

/// \brief Структура события в бинарном файле журнала.
struct EventRecord
{
    quint64 time;   ///< UNIX Time64
    EventType type; ///< Тип события
    quint8 num[3];  ///< ID события (используется для описания)
    quint32 reserv; ///< Дополнительная информация в HEX
};

/// \brief Структура события для отображения в ETableModel.
struct EventView
{
    int counter;       ///< Номер события
    quint64 time;      ///< UNIX Time64
    QString desc;      ///< Описание события
    QString direction; ///< Тип события
    QString hexField;  ///< Дополнительное поле
};

using Descriptions = ModuleTypes::WorkJourMap;
using Data = QVector<QVector<QVariant>>;

/// \brief Класс для парсинга событий из системных и рабочих журналов.
class EventParser : public QObject
{
    Q_OBJECT
private:
    const EventRecord *iter;
    std::size_t size;
    QVector<EventView> records;

    /// \brief Возвращает строковое представление  перечисления типа события.
    QString eventTypeToString(const EventType type);

public:
    EventParser(QObject *parent = nullptr);

    /// \brief По полученному массиву байт получаем итератор и количество событий в файле.
    /// \param ba[in] - массив байт бинарного файла журнала.
    void update(const QByteArray &ba);

    /// \brief Функция парсинга полученного файла в представление для ETableModel.
    /// \param desc[in] - хеш-карта, key - ID события, value - строка с описанием события.
    /// \param timeZone[in] - часовой пояс для получения корректного времени событий в журнале.
    const Data parse(const Descriptions &desc, const QTimeZone timeZone);
};

}
