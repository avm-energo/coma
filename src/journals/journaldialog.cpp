#include "journaldialog.h"

#include "measjournal.h"
#include "sysjournal.h"
#include "workjournal.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <s2/s2datatypes.h>

namespace journals
{

JournalDialog::JournalDialog(const ModuleSettings &settings, QWidget *parent) : UDialog(parent)
{
    disableSuccessMessage();
    m_dataUpdater->currentConnection()->connection(this, &JournalDialog::receivedJournalFile);
    createJournals(settings);
    setupUI();
}

void JournalDialog::createJournals(const ModuleSettings &settings)
{
    using namespace journals;
    auto sysJour = new SysJournal(this);
    auto sysJourTab = new JournalTabWidget(sysJour, this);
    journals.insert({ sysJour->getType(), sysJourTab });
    if (!settings.getWorkJours().isEmpty())
    {
        auto workJour = new WorkJournal(settings.getWorkJours(), this);
        auto workJourTab = new JournalTabWidget(workJour, this);
        journals.insert({ workJour->getType(), workJourTab });
    }
    if (!settings.getMeasJours().empty())
    {
        auto measJour = new MeasJournal(settings.getMeasJours(), this);
        auto measJourTab = new JournalTabWidget(measJour, this);
        journals.insert({ measJour->getType(), measJourTab });
    }
}

void JournalDialog::setupUI()
{
    auto layout = new QVBoxLayout;
    auto tabWidget = new QTabWidget(this);
    for (const auto &journal : journals)
    {
        auto journalTab = journal.second;
        tabWidget->addTab(journalTab, journalTab->getTabName());
    }
    layout->addWidget(tabWidget);
    setLayout(layout);
}

void JournalDialog::receivedJournalFile(const S2::S2BFile &s2bFile)
{
    auto jourType = static_cast<JournalType>(s2bFile.header.fname);
    auto search = journals.find(jourType);
    if (search != journals.end())
        search->second->setJournalFile(s2bFile);
}

}
