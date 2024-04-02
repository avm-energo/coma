#pragma once

#include "../../widgets/udialog.h"

#include <journals/journaltabwidget.h>
#include <map>

namespace journals
{

/// \brief Класс для отображения JournalTabWidget.
/// \see JournalTabWidget
class JournalDialog : public UDialog
{
    Q_OBJECT
public:
    explicit JournalDialog(Device::CurrentDevice *device, QWidget *parent = nullptr);

private:
    std::map<JournalType, JournalTabWidget *> m_journals;

    /// \brief Метод для создания объектов журналов по полученным настройкам.
    /// \param settings[in] - настройки, полученные от DialogCreator.
    /// \see DialogCreator
    void createJournalTabs();

    /// \brief Метод для создания UI отображаемого диалога.
    void setupUI();

    /// \brief Метод для создания объекта журнала указанного типа с указанным родителем.
    BaseJournal *createJournal(JournalType type, QObject *parent = nullptr) noexcept;

private slots:
    /// \brief Слот для получения файла журнала от устройства.
    /// \details В зависимости от указанного типа журнала, пересылает
    /// журнал в формате S2B одной из вкладок.
    void createJournalAndFillModel(const S2::S2BFile &journalFile);
};

}
