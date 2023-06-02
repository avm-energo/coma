#include "journaltabwidget.h"

#include "../interfaces/baseinterface.h"
//#include "../widgets/wd_func.h"
#include "keypressdialog.h"

#include <QPushButton>
#include <QVBoxLayout>

namespace Test
{

JournalTabWidget::JournalTabWidget(Journal *jour, QWidget *parent)
    : QWidget(parent)
    , journal(jour)
    , modelView(journal->createModelView(this))
    , getProgressIndicator(nullptr)
    , getProgressDialog(nullptr)
    , saveProgressDialog(new QProgressDialog(this))
{
    setupProgressDialogs();
    setupUI();
}

void JournalTabWidget::setupProgressDialogs()
{
    constexpr auto progressDlgParams = //
        Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint;
    getProgressDialog = new QDialog(this, progressDlgParams);
    auto progressLayout = new QHBoxLayout;
    getProgressIndicator = new QProgressIndicator(this);
    progressLayout->addWidget(getProgressIndicator);
    getProgressDialog->setLayout(progressLayout);
    saveProgressDialog->setCancelButton(nullptr);
    saveProgressDialog->cancel();
    connect(journal, &Journal::resendMaxResult, saveProgressDialog, &QProgressDialog::setMaximum);
    connect(journal, &Journal::resendResult, saveProgressDialog, &QProgressDialog::setValue);
}

void JournalTabWidget::setupUI()
{
    auto hLayout = new QHBoxLayout;
    auto vLayout = new QVBoxLayout;
    auto str = journal->getName().toLower();

    auto getButton = new QPushButton("Получить " + str, this);
    connect(getButton, &QPushButton::clicked, this, &JournalTabWidget::getJournal);
    hLayout->addWidget(getButton);

    auto eraseButton = new QPushButton("Стереть " + str, this);
    connect(eraseButton, &QPushButton::clicked, this, &JournalTabWidget::eraseJournal);
    hLayout->addWidget(eraseButton);

    auto saveButton = new QPushButton("Сохранить журнал в файл", this);
    connect(saveButton, &QPushButton::clicked, this, &JournalTabWidget::saveJournal);
    hLayout->addWidget(saveButton);

    connect(journal, &Journal::done, this, [this, saveButton] {
        getProgressDialog->close();
        modelView->setUpdatesEnabled(true);
        saveButton->setEnabled(true);
    });

    vLayout->addLayout(hLayout);
    vLayout->addWidget(modelView, 89);
    setLayout(vLayout);
}

void JournalTabWidget::getJournal()
{
    getProgressDialog->show();
    getProgressIndicator->startAnimation();
    Interface::BaseInterface::iface()->reqFile(quint32(journal->getType()));
    modelView->setUpdatesEnabled(false);
}

void JournalTabWidget::eraseJournal()
{
    using namespace Interface;
    // Manual checking password
    static constexpr auto hash = "d93fdd6d1fb5afcca939fa650b62541d09dbcb766f41c39352dc75f348fb35dc";
    auto kpd = new KeyPressDialog;
    if (kpd->CheckPassword(hash))
        BaseInterface::iface()->writeCommand(Commands::C_EraseJournals, quint8(journal->getType()));
}

void JournalTabWidget::saveJournal()
{
    ;
}

void JournalTabWidget::setJournalFile(const S2DataTypes::S2BFile &jourFile)
{
    auto storedType = quint32(journal->getType());
    if (storedType == jourFile.file.ID)
    {
        journalFile = jourFile;
        // journal->fill();
    }
}

}
