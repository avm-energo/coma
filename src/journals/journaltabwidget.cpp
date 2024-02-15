#include "journaltabwidget.h"

//#include "../module/board.h"
#include "../widgets/epopup.h"
#include "../widgets/wd_func.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <gen/files.h>
#include <interfaces/conn/active_connection.h>
#include <map>

namespace journals
{

const std::map<JournalType, QString> JournalTabWidget::s_prefixByType {
    { JournalType::System, "SysJ" }, //
    { JournalType::Work, "WorkJ" },  //
    { JournalType::Meas, "MeasJ" }   //
};

JournalTabWidget::JournalTabWidget(const JournalType type, QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout)
    , m_modelView(nullptr)
    , m_progressIndicator(nullptr)
    , m_progressDialog(nullptr)
    , m_saveProgressDialog(new QProgressDialog(this))
    , m_type(type)
{
    setupProgressDialogs();
    setupUI();
}

void JournalTabWidget::setupProgressDialogs()
{
    constexpr auto progressDlgParams = //
        Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint;
    m_progressDialog = new QDialog(this, progressDlgParams);
    auto progressLayout = new QHBoxLayout;
    m_progressIndicator = new EProgressIndicator(this);
    progressLayout->addWidget(m_progressIndicator);
    m_progressDialog->setLayout(progressLayout);
    m_saveProgressDialog->setCancelButton(nullptr);
    m_saveProgressDialog->cancel();
}

void JournalTabWidget::setupUI()
{
    auto hLayout1 = new QHBoxLayout;
    auto hLayout2 = new QHBoxLayout;
    auto str = getTabName().toLower();

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

    connect(this, &JournalTabWidget::ready, this, [this, saveExcelButton, saveBinaryButton] {
        m_progressDialog->close();
        m_modelView->setUpdatesEnabled(true);
        saveExcelButton->setEnabled(true);
        saveBinaryButton->setEnabled(true);
    });

    m_mainLayout->setAlignment(Qt::AlignTop);
    m_mainLayout->addLayout(hLayout1);
    m_mainLayout->addLayout(hLayout2);
    setLayout(m_mainLayout);
}

QString JournalTabWidget::getSuggestedFilename()
{
    QString suggestedFilename = "";
    auto search = s_prefixByType.find(m_type);
    if (search != s_prefixByType.cend())
    {
        const auto &journalFile = m_currentJournal->getFile();
        suggestedFilename = search->second + " ";
        suggestedFilename += QString::number(journalFile.header.typeB, 16);
        suggestedFilename += QString::number(journalFile.header.typeM, 16) + " #";
        suggestedFilename += QString("%1").arg(journalFile.tail.serialnum, 8, 10, QChar('0'));
        suggestedFilename += " " + QDate::currentDate().toString("dd-MM-yyyy");
    }
    return suggestedFilename;
}

void JournalTabWidget::gettingJournal()
{
    if (m_modelView != nullptr)
        m_modelView->deleteLater();
    m_progressDialog->show();
    m_progressIndicator->startAnimation();
    ActiveConnection::async()->reqFile(static_cast<quint16>(m_type));
}

void JournalTabWidget::eraseJournal()
{
    if (EMessageBox::password(this))
        ActiveConnection::async()->writeCommand(Interface::Commands::C_EraseJournals, static_cast<quint16>(m_type));
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
            m_saveProgressDialog->setMinimumDuration(0);
            m_currentJournal->saveToExcel(filename);
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
            const auto &journalFile = m_currentJournal->getFile();
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
    emit ready();
    EMessageBox::information(this, message);
}

void JournalTabWidget::error(const QString &message)
{
    EMessageBox::error(this, message);
    qCritical() << message;
}

void JournalTabWidget::update(BaseJournal *newJournal)
{
    if (newJournal == nullptr)
        return;

    connect(newJournal, &BaseJournal::done, this, &JournalTabWidget::done);
    connect(newJournal, &BaseJournal::error, this, &JournalTabWidget::error);
    connect(newJournal, &BaseJournal::resendMaxResult, m_saveProgressDialog, &QProgressDialog::setMaximum);
    connect(newJournal, &BaseJournal::resendResult, m_saveProgressDialog, &QProgressDialog::setValue);
    m_modelView = newJournal->createModelView(this);
    m_modelView->setUpdatesEnabled(false);
    m_mainLayout->addWidget(m_modelView, 89);
    m_currentJournal.reset(newJournal);
}

QString JournalTabWidget::getTabName() const
{
    return BaseJournal::getJournalName(m_type);
}

}
