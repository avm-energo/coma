#pragma once

#include "../widgets/udialog.h"
#include "journaltabwidget.h"

#include <interfaces/utils/typesproxy.h>
#include <map>

namespace journals
{

/// \brief Класс для отображения JournalTabWidget.
/// \see JournalTabWidget
class JournalDialog : public UDialog
{
    Q_OBJECT
public:
    JournalDialog(const ModuleSettings &settings, QWidget *parent = nullptr);

private:
    DataTypesProxy proxy;
    std::map<JournalType, JournalTabWidget *> journals;

    /// \brief Метод для создания объектов журналов по полученным настройкам.
    /// \param settings[in] - настройки, полученные от DialogCreator.
    /// \see DialogCreator
    void createJournals(const ModuleSettings &settings);

    /// \brief Метод для создания UI отображаемого диалога.
    void setupUI();

private slots:
    /// \brief Слот для получения файла журнала от устройства.
    /// \details В зависимости от указанного типа журнала, пересылает
    /// журнал в формате S2B одной из вкладок.
    void receivedJournalFile(const QVariant &jourData);
};

}
