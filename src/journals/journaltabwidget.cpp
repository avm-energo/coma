#include "journaltabwidget.h"

#include "../dialogs/keypressdialog.h"
#include "../module/board.h"
#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <gen/files.h>
#include <interfaces/conn/active_connection.h>
#include <map>

namespace journals
{

JournalTabWidget::JournalTabWidget(BaseJournal *jour, QWidget *parent)
    : QWidget(parent)
    , journal(jour)
    , modelView(journal->createModelView(this))
    , progressIndicator(nullptr)
    , progressDialog(nullptr)
    , saveProgressDialog(new QProgressDialog(this))
{
    setupProgressDialogs();
    setupUI();
    connect(journal, &BaseJournal::done, this, &JournalTabWidget::done);
    connect(journal, &BaseJournal::error, this, &JournalTabWidget::error);
}

void JournalTabWidget::setupProgressDialogs()
{
    constexpr auto progressDlgParams = //
        Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint;
    progressDialog = new QDialog(this, progressDlgParams);
    auto progressLayout = new QHBoxLayout;
    progressIndicator = new EProgressIndicator(this);
    progressLayout->addWidget(progressIndicator);
    progressDialog->setLayout(progressLayout);
    saveProgressDialog->setCancelButton(nullptr);
    saveProgressDialog->cancel();
    connect(journal, &BaseJournal::resendMaxResult, saveProgressDialog, &QProgressDialog::setMaximum);
    connect(journal, &BaseJournal::resendResult, saveProgressDialog, &QProgressDialog::setValue);
}

void JournalTabWidget::setupUI()
{
    auto hLayout1 = new QHBoxLayout;
    auto hLayout2 = new QHBoxLayout;
    auto vLayout = new QVBoxLayout;
    auto str = journal->getName().toLower();

    auto getButton = new QPushButton("Получить " + str, this);
    connect(getButton, &QPushButton::clicked, this, &JournalTabWidget::gettingJournal);
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

    connect(journal, &BaseJournal::done, this, [this, saveExcelButton, saveBinaryButton] {
        progressDialog->close();
        modelView->setUpdatesEnabled(true);
        saveExcelButton->setEnabled(true);
        saveBinaryButton->setEnabled(true);
    });

    vLayout->addLayout(hLayout1);
    vLayout->addLayout(hLayout2);
    vLayout->addWidget(modelView, 89);
    setLayout(vLayout);
}

QString JournalTabWidget::getSuggestedFilename()
{
    QString suggestedFilename = "";
    static const std::map<JournalType, QString> names {
        { JournalType::System, "SysJ" }, //
        { JournalType::Work, "WorkJ" },  //
        { JournalType::Meas, "MeasJ" }   //
    };
    auto search = names.find(journal->getType());
    if (search != names.end())
    {
        const auto &board = Board::GetInstance();
        suggestedFilename = search->second + " ";
        suggestedFilename += QString::number(journalFile.header.typeB, 16);
        suggestedFilename += QString::number(journalFile.header.typeM, 16) + " #";
        suggestedFilename += QString("%1").arg(board.serialNumber(Board::BaseAdd), 8, 10, QChar('0'));
        suggestedFilename += " " + QDate::currentDate().toString("dd-MM-yyyy");
    }
    return suggestedFilename;
}

void JournalTabWidget::gettingJournal()
{
    progressDialog->show();
    progressIndicator->startAnimation();
    ActiveConnection::async()->reqFile(quint32(journal->getType()));
    modelView->setUpdatesEnabled(false);
}

void JournalTabWidget::eraseJournal()
{
    // Manual checking password
    static constexpr auto hash = "d93fdd6d1fb5afcca939fa650b62541d09dbcb766f41c39352dc75f348fb35dc";
    auto kpd = new KeyPressDialog;
    if (kpd->CheckPassword(hash))
        ActiveConnection::async()->writeCommand(Interface::Commands::C_EraseJournals, quint8(journal->getType()));
}

void JournalTabWidget::saveExcelJournal()
{
    auto suggestedFilename = getSuggestedFilename();
    if (!suggestedFilename.isEmpty())
    {
        suggestedFilename += ".xlsx";
        auto filename = WDFunc::ChooseFileForSave(this, "Excel documents (*.xlsx)", "xlsx", suggestedFilename);
        if (!filename.isEmpty())
        {
            saveProgressDialog->setMinimumDuration(0);
            journal->save(filename);
        }
    }
}

void JournalTabWidget::saveBinaryJournal()
{
    auto suggestedFilename = getSuggestedFilename();
    if (!suggestedFilename.isEmpty())
    {
        suggestedFilename += ".jn";
        auto filename = WDFunc::ChooseFileForSave(this, "Journal files (*.jn)", "jn", suggestedFilename);
        if (!filename.isEmpty())
        {
            auto &fileHeader = journalFile.header;
            auto file = QByteArray::fromRawData(reinterpret_cast<const char *>(&fileHeader), sizeof(fileHeader));
            file.append(journalFile.data);
            auto &fileTail = journalFile.tail;
            file.append(QByteArray::fromRawData(reinterpret_cast<const char *>(&fileTail), sizeof(fileTail)));
            Files::SaveToFile(filename, file);
            done("Файл создан успешно");
        }
    }
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

void JournalTabWidget::setJournalFile(const S2::S2BFile &jourFile)
{
    auto storedType = quint32(journal->getType());
    if (storedType == jourFile.header.fname)
    {
        journalFile = jourFile;
        journal->fill(journalFile.data);
    }
}

const QString &JournalTabWidget::getTabName() const
{
    return journal->getName();
}

}
