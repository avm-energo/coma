#pragma once

#include "../module/modulesettings.h"

#include <QTimeZone>
#include <gen/timefunc.h>

namespace journals
{

using MeasSettings = ModuleTypes::MeasJourList;
using Data = QVector<QVector<QVariant>>;

/// \brief Класс для парсинга событий из журналов измерений.
class MeasParser : public QObject
{
    Q_OBJECT
private:
    QTimeZone timezone;
    const quint8 *iter;
    std::size_t recordSize;
    std::size_t size;
    int timeIndex;
    QVector<QVariant> record;

    /// \brief Парсинг одной записи из файла журнала.
    /// \details В данном случае под одной записью понимается одна строка с данными
    /// для модели журнала измерений и её отображения.
    /// \return true если были получены корректные данные, false в обратном случае.
    bool parseRecord(const MeasSettings &settings);

    /// \brief Получение размера одной записи из файла журнала по полученному описанию модели.
    std::size_t getRecordSize(const MeasSettings &settings);

    int findTimeIndex(const MeasSettings &settings);
    bool sortByTime(const QVector<QVariant> &lhs, const QVector<QVariant> &rhs);

    template <class T> QVariant iterateValue()
    {
        auto value = *reinterpret_cast<const T *>(iter);
        iter += sizeof(T);
        return value;
    }

    template <class T> QVariant iterateTime(bool &status)
    {
        constexpr auto max = std::numeric_limits<T>().max();
        auto value = *reinterpret_cast<const T *>(iter);
        iter += sizeof(T);
        if (value == max)
            status = false;
        if constexpr (std::is_same_v<T, quint32>)
            return TimeFunc::UnixTime32ToInvString(value, timezone);
        else if constexpr (std::is_same_v<T, quint64>)
            return TimeFunc::UnixTime64ToInvStringFractional(value, timezone);
        else
        {
            status = false;
            return QVariant();
        }
    }

public:
    explicit MeasParser(QTimeZone timeZone, QObject *parent = nullptr);
    void update(const QByteArray &ba, const MeasSettings &settings);
    const Data parse(const MeasSettings &settings);
};

}
