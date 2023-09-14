#pragma once

#include "../widgets/QProgressIndicator.h"
#include "basejournal.h"

#include <QDialog>
#include <QProgressDialog>
#include <QWidget>
#include <s2/s2datatypes.h>

namespace journals
{

/// \brief Класс виджета для вкладки, отображающей журнал.
class JournalTabWidget : public QWidget
{
    Q_OBJECT
private:
    BaseJournal *journal;
    ETableView *modelView;
    QDialog *getProgressDialog;
    QProgressIndicator *getProgressIndicator;
    QProgressDialog *saveProgressDialog;
    S2::S2BFile journalFile;

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
    JournalTabWidget(BaseJournal *jour, QWidget *parent = nullptr);

    /// \brief Обновление содержимого виджета по полученному файлу журнала.
    /// \param jourFile[in] - файл журнала в формате S2B, полученный от устройства.
    void setJournalFile(const S2::S2BFile &jourFile);

    /// \brief Функция, возвращающая имя вкладки.
    const QString &getTabName() const;
};

}
