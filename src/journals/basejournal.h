#pragma once

#include "../models/edynamictablemodel.h"
#include "../module/modulesettings.h"
#include "../widgets/etableview.h"

#include <QObject>
#include <QSortFilterProxyModel>
#include <QTimeZone>
#include <gen/stdfunc.h>

namespace journals
{
/// \brief Перечисление для типа журнала.
enum JournalType : quint8
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
    const static QStringList eventJourHeaders;

    QString jourName;
    QString viewName;
    QStringList headers;
    JournalType type;
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

    /// \brief Возвращает имя журнала.
    /// \return Возвращает строку "Системный журнал", "Рабочий журнал"
    /// или "Журнал измерений" в зависимости от типа журнала.
    const QString &getName() const;

    /// \brief Возвращает имя объекта ETableView для текущего журнала.
    /// \return Возвращает строку "system", "work" или "meas" в зависимости от типа журнала.
    const QString &getViewName() const;

    /// \brief Возвращает тип текущего журнала.
    const JournalType getType() const;

signals:
    /// \brief Сигнал об успешном выполнении операции.
    void done(const QString &msg);
    /// \brief Сигнал об ошибке во время выполнения операции.
    void error(const QString &msg);
    /// \brief Состояние выполнения текущей операции.
    void resendResult(const int result);
    /// \brief Число шагов выполняющейся операции.
    void resendMaxResult(const int max);

public slots:
    /// \brief Заполняет модель данными, приходящими извне.
    /// \param data[in] - FileStruct, содержащий прочитанный бинарный файл журнала.
    void fill(const QVariant &data);

    /// \brief Сохраняет модель в Excel-файл с указанным именем.
    void save(const QString &filename);
};

}
