#pragma once

#include "../widgets/eprogressindicator.h"
#include "basejournal.h"

#include <QDialog>
#include <QProgressDialog>
#include <QWidget>
#include <s2/s2datatypes.h>

class QVBoxLayout;

namespace journals
{

/// \brief Класс виджета для вкладки, отображающей журнал.
class JournalTabWidget : public QWidget
{
    Q_OBJECT
private:
    static const std::map<JournalType, QString> s_prefixByType;

    UniquePointer<BaseJournal> journal;
    QVBoxLayout *m_mainLayout;
    ETableView *modelView;
    QDialog *progressDialog;
    EProgressIndicator *progressIndicator;
    QProgressDialog *saveProgressDialog;
    JournalType m_type;

    /// \brief Метод для создания виджетов, сигнализирующих
    /// пользователю состояние обмена данными и сохранения данных в файл.
    void setupProgressDialogs();

    /// \brief Создание UI виджета: кнопки, слои, TableViewModel.
    void setupUI();

    /// \brief Функция, возвращающая рекомендуемое имя файла.
    /// \see saveExcelJournal, saveBinaryJournal
    QString getSuggestedFilename();

private slots:
    /// \brief Слот для получения журнала от устройства по нажатию на кнопку.
    void gettingJournal();
    /// \brief Слот для очистки журнала по нажатию на кнопку.
    void eraseJournal();

    /// \brief Слот для сохранения журнала в Excel-таблицу.
    void saveExcelJournal();
    /// \brief Слот для сохранения журнала в бинарный файл.
    void saveBinaryJournal();

    /// \brief Слот для отображения сообщений об успешной обработке данных.
    void done(const QString &message);
    /// \brief Слот для отображения сообщений об ошибках.
    void error(const QString &message);

public:
    explicit JournalTabWidget(const JournalType type, QWidget *parent = nullptr);

    /// \brief Обновление содержимого виджета по полученному объекту журнала.
    /// \param newJournal[in] - журнала, полученный от родительского класса JournalDialog.
    void update(BaseJournal *newJournal);

    /// \brief Функция, возвращающая имя вкладки.
    QString getTabName() const;

signals:
    /// \brief Сигнал для уведомления UI о том, что журнал загружен успешно.
    void ready();
};

}
