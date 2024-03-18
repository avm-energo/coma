#pragma once

#include "../module/modulesettings.h"

#include <QTimeZone>
#include <gen/timefunc.h>

namespace journals
{

using MeasSettings = ModuleTypes::MeasJourList;
using JournalData = QVector<QVector<QVariant>>;

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

    /// \brief Получение размера одной записи из файла журнала измерений.
    /// \param settings[in] - полученное описание модели.
    std::size_t getRecordSize(const MeasSettings &settings);

    /// \brief Получение индекса столбца с временем записи измерений.
    /// \param settings[in] - полученное описание модели.
    int findTimeIndex(const MeasSettings &settings);

    /// \brief Компаратор записей измерений для передачи в функцию сортировки.
    /// \details Одна запись с измерениями представляет собой последовательность
    /// экземпляров класса QVariant. Если известен индекс столбца, в котором
    /// указано время записи измерений, то по этому индексу можно получить значение,
    /// по которому записи с измерениями можно отсортировать. Данная функция
    /// используется для сортировки времени замеров измерений по убыванию: от
    /// более младших к более младшим.
    /// \see findTimeIndex
    bool sortByTime(const QVector<QVariant> &lhs, const QVector<QVariant> &rhs);

    /// \brief Возвращает простой тип из файла журнала измерений.
    /// \details Значение считывается из итератора по бинарному файлу,
    /// при этом итератор осуществляет сдвиг вперёд на количество байт,
    /// равное длине указанном простого типа T.
    template <class T> QVariant iterateValue()
    {
        auto value = *reinterpret_cast<const T *>(iter);
        iter += sizeof(T);
        return value;
    }

    /// \brief Возвращает строковое представление UNIX Datetime.
    /// \details Значение считывается из итератора по бинарному файлу,
    /// при этом итератор осуществляет сдвиг вперёд на количество байт,
    /// равное длине указанном простого типа T.
    /// \return Строковое представление времени измерений, обёрнутое в QVariant.
    /// В случае ошибки возвращается пустой объект класса QVariant.
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

    /// \brief Инициализация настроек парсера.
    /// \param ba[in] - массив байт бинарного файла журнала.
    /// \param settings[in] - полученное описание модели журнала измерений.
    /// \details По полученному массиву байт получаем итератор по файлу журнала.
    /// По полученному описанию модели получаем размер одной записи в журнале.
    /// Исходя из размера файла и размера одной записи, получаем количество записей в файле.
    /// Также выполняем поиск индекса столбца с временем записи измерений.
    /// \see getRecordSize, findTimeIndex
    void update(const QByteArray &ba, const MeasSettings &settings);

    /// \brief Функция парсинга полученного файла в представление для ETableModel.
    /// \param settings[in] - полученное описание модели журнала измерений.
    /// \details После парсинга записей жунрала измерений производится сортировка
    /// этих записей, если найден индекса столбца с временем записи измерений.
    /// \see parseRecord, iterateValue, iterateTime, sortByTime
    const JournalData parse(const MeasSettings &settings);
};

}
