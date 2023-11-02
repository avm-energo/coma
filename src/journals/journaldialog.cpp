#include "journaldialog.h"

#include "../s2/s2datatypes.h"
#include "measjournal.h"
#include "sysjournal.h"
#include "workjournal.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <gen/datamanager/typesproxy.h>

namespace journals
{

JournalDialog::JournalDialog(const ModuleSettings &settings, QWidget *parent)
    : UDialog(parent), proxy(&DataManager::GetInstance())
{
    disableSuccessMessage();
    proxy.RegisterType<S2::S2BFile>();
    connect(&proxy, &DataTypesProxy::DataStorable, this, &JournalDialog::receivedJournalFile);
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

void JournalDialog::receivedJournalFile(const QVariant &jourData)
{
    if (jourData.canConvert<S2::S2BFile>())
    {
        auto s2bFile = jourData.value<S2::S2BFile>();
        auto jourType = static_cast<JournalType>(s2bFile.header.fname);
        auto search = journals.find(jourType);
        if (search != journals.end())
            search->second->setJournalFile(s2bFile);
    }
}

}
