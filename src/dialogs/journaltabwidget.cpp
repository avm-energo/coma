#include "journaltabwidget.h"

#include "../interfaces/baseinterface.h"
#include "../module/board.h"
#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"
#include "keypressdialog.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <map>

namespace journals
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
    connect(journal, &Journal::done, this, &JournalTabWidget::done);
    connect(journal, &Journal::error, this, &JournalTabWidget::error);
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
    auto hLayout1 = new QHBoxLayout;
    auto hLayout2 = new QHBoxLayout;
    auto vLayout = new QVBoxLayout;
    auto str = journal->getName().toLower();

    auto getButton = new QPushButton("Получить " + str, this);
    connect(getButton, &QPushButton::clicked, this, &JournalTabWidget::getJournal);
    hLayout1->addWidget(getButton);

    auto eraseButton = new QPushButton("Стереть " + str, this);
    connect(eraseButton, &QPushButton::clicked, this, &JournalTabWidget::eraseJournal);
    hLayout1->addWidget(eraseButton);

    auto saveExcelButton = new QPushButton("Сохранить журнал в Excel файл", this);
    connect(saveExcelButton, &QPushButton::clicked, this, &JournalTabWidget::saveExcelJournal);
    saveExcelButton->setEnabled(false);
    hLayout2->addWidget(saveExcelButton);

    auto saveBinaryButton = new QPushButton("Сохранить журнал в бинарный файл", this);
    connect(saveBinaryButton, &QPushButton::clicked, this, &JournalTabWidget::saveBinaryJournal);
    saveBinaryButton->setEnabled(false);
    hLayout2->addWidget(saveBinaryButton);

    connect(journal, &Journal::done, this, [this, saveExcelButton, saveBinaryButton] {
        getProgressDialog->close();
        modelView->setUpdatesEnabled(true);
        saveExcelButton->setEnabled(true);
        saveBinaryButton->setEnabled(true);
    });

    vLayout->addLayout(hLayout1);
    vLayout->addLayout(hLayout2);
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

void JournalTabWidget::saveExcelJournal()
{
    static const std::map<JournalType, QString> names {
        { JournalType::System, "SysJ" }, //
        { JournalType::Work, "WorkJ" },  //
        { JournalType::Meas, "MeasJ" }   //
    };
    auto search = names.find(journal->getType());
    if (search != names.end())
    {
        const auto &board = Board::GetInstance();
        auto suggestedFilename = search->second + " ";
        suggestedFilename += QString::number(journalFile.header.typeB, 16);
        suggestedFilename += QString::number(journalFile.header.typeM, 16) + " #";
        suggestedFilename += QString("%1").arg(board.serialNumber(Board::BaseAdd), 8, 10, QChar('0'));
        suggestedFilename += " " + QDate::currentDate().toString("dd-MM-yyyy") + ".xlsx";
        auto filename = WDFunc::ChooseFileForSave(nullptr, "Excel documents (*.xlsx)", "xlsx", suggestedFilename);
        saveProgressDialog->setMinimumDuration(0);
        journal->save(filename);
    }
}

void JournalTabWidget::saveBinaryJournal()
{
    ;
}

void JournalTabWidget::done(const QString &message)
{
    EMessageBox::information(this, message);
}

void JournalTabWidget::error(const QString &message)
{
    EMessageBox::error(this, message);
    qCritical() << message;
}

void JournalTabWidget::setJournalFile(const S2DataTypes::S2BFile &jourFile)
{
    auto storedType = quint32(journal->getType());
    if (storedType == jourFile.file.ID)
    {
        journalFile = jourFile;
        journal->fill(journalFile.file);
    }
}

const QString &JournalTabWidget::getJournalName() const
{
    return journal->getName();
}

}
