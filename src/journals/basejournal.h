#pragma once

#include "../module/modulesettings.h"

#include <QObject>
#include <QTimeZone>
#include <gen/stdfunc.h>
#include <s2/s2datatypes.h>

class EDynamicTableModel;
class ETableView;
class QSortFilterProxyModel;

namespace journals
{
/// \brief Перечисление для типа журнала.
enum class JournalType : quint8
{
    System = 4,
    Work = 5,
    Meas = 6
};

/// \brief Абстрактный базовый класс журнала.
class BaseJournal : public QObject
{
    Q_OBJECT
protected:
    static const QStringList s_eventJourHeaders;
    static const std::map<JournalType, QString> s_nameByType;
    static const std::map<JournalType, QString> s_prefixByType;

    S2::S2BFile m_file;
    QString viewName;
    QStringList headers;
    QTimeZone timezone;
    UniquePointer<EDynamicTableModel> dataModel;
    UniquePointer<QSortFilterProxyModel> proxyModel;

    /// \brief Форматирует заголовки в соответствии с текущим часовым поясом.
    /// \details В полученном списке строк data находит строку,
    /// содержащую "UTC", и заменяет её на текущий часовой пояс
    /// (например, "UTC+03:00"). Используется для форматирования
    /// заголовков в отображении модели.
    void setUserTimezone(QStringList &data);

    /// \brief Заполняет модель данными, от полученного бинарного файла.
    /// \param ba[in] - массив байт бинарного файла журнала.
    virtual void fillModel(const QByteArray &ba) = 0;

public:
    explicit BaseJournal(QObject *parent = nullptr);

    /// \brief Создаёт объект-потомок ETableView по указателю на его родительский объект.
    /// \see JournalDialog
    ETableView *createModelView(QWidget *parent) const;

    /// \brief Заполняет модель данными, приходящими извне.
    /// \param journalFile[in] - файл в формате S2B, содержащий прочитанный бинарный файл журнала.
    void fill(const S2::S2BFile &journalFile);

    const S2::S2BFile &getFile() const noexcept;

    /// \brief Сохраняет модель в Excel-файл с указанным именем.
    void saveToExcel(const QString &filename);

    /// \brief Возвращает имя журнала по его типу.
    static const QString &getJournalName(const JournalType type, const QString &defaultValue = "") noexcept;

signals:
    /// \brief Сигнал об успешном выполнении операции.
    void done(const QString &msg);
    /// \brief Сигнал об ошибке во время выполнения операции.
    void error(const QString &msg);
    /// \brief Состояние выполнения текущей операции.
    void resendResult(const int result);
    /// \brief Число шагов выполняющейся операции.
    void resendMaxResult(const int max);
};

}
