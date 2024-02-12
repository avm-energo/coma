#include "journaldialog.h"

#include "journaltabwidget.h"
#include "measjournal.h"
#include "sysjournal.h"
#include "workjournal.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <s2/s2datatypes.h>

namespace journals
{

JournalDialog::JournalDialog(const ModuleSettings &settings, QWidget *parent) : UDialog(parent), m_settings(settings)
{
    disableSuccessMessage();
    m_dataUpdater->currentConnection()->connection(this, &JournalDialog::receivedJournalFile);
    createJournalTabs();
    setupUI();
}

void JournalDialog::createJournalTabs()
{
    using namespace journals;
    auto journalType = JournalType::System;
    auto sysJourTab = new JournalTabWidget(journalType, this);
    m_journals.insert({ journalType, sysJourTab });
    if (!m_settings.get().getWorkJours().isEmpty())
    {
        journalType = JournalType::Work;
        auto workJourTab = new JournalTabWidget(journalType, this);
        m_journals.insert({ journalType, workJourTab });
    }
    if (!m_settings.get().getMeasJours().empty())
    {
        journalType = JournalType::Meas;
        auto measJourTab = new JournalTabWidget(journalType, this);
        m_journals.insert({ journalType, measJourTab });
    }
}

void JournalDialog::setupUI()
{
    auto layout = new QVBoxLayout;
    auto tabWidget = new QTabWidget(this);
    for (const auto &journal : m_journals)
    {
        auto journalTab = journal.second;
        tabWidget->addTab(journalTab, journalTab->getTabName());
    }
    layout->addWidget(tabWidget);
    setLayout(layout);
}

BaseJournal *JournalDialog::createJournal(JournalType type, QObject *parent) noexcept
{
    BaseJournal *journal = nullptr;
    switch (type)
    {
    case JournalType::System:
        journal = new SysJournal(parent);
        break;
    case JournalType::Work:
        journal = new WorkJournal(m_settings.get().getWorkJours(), parent);
        break;
    case JournalType::Meas:
        /// TODO: make journal uwidget
        // journal = new MeasJournal(m_settings.get().getMeasJours(), parent);
        break;
    default:
        journal = nullptr;
        break;
    }
    return journal;
}

void JournalDialog::receivedJournalFile(const S2::S2BFile &journalFile)
{
    auto journalType = static_cast<JournalType>(journalFile.header.fname);
    auto search = m_journals.find(journalType);
    if (search != m_journals.end())
    {
        auto journal = createJournal(search->first, search->second);
        if (journal != nullptr)
        {
            search->second->update(journal);
            journal->fill(journalFile);
        }
    }
}

}
