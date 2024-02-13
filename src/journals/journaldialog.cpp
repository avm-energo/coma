#include "journaldialog.h"

#include "journaltabwidget.h"
#include "measjournal.h"
#include "sysjournal.h"
#include "workjournal.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <device/current_device.h>
#include <s2/s2datatypes.h>

namespace journals
{

JournalDialog::JournalDialog(Device::CurrentDevice *device, QWidget *parent) : UDialog(device, parent)
{
    disableSuccessMessage();
    m_device->async()->connection(this, &JournalDialog::receivedJournalFile);
    createJournalTabs();
    setupUI();
}

void JournalDialog::createJournalTabs()
{
    using namespace journals;
    auto journalType = JournalType::System;
    auto sysJourTab = new JournalTabWidget(journalType, m_device->async(), this);
    m_journals.insert({ journalType, sysJourTab });
    auto &settings = m_device->getConfigStorage()->getDeviceSettings();
    if (!settings.getWorkJours().isEmpty())
    {
        journalType = JournalType::Work;
        auto workJourTab = new JournalTabWidget(journalType, m_device->async(), this);
        m_journals.insert({ journalType, workJourTab });
    }
    if (!settings.getMeasJours().empty())
    {
        journalType = JournalType::Meas;
        auto measJourTab = new JournalTabWidget(journalType, m_device->async(), this);
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
    auto &settings = m_device->getConfigStorage()->getDeviceSettings();
    switch (type)
    {
    case JournalType::System:
        journal = new SysJournal(parent);
        break;
    case JournalType::Work:
        journal = new WorkJournal(settings.getWorkJours(), parent);
        break;
    case JournalType::Meas:
        journal = new MeasJournal(settings.getMeasJours(), parent);
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
