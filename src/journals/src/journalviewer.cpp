#include "journals/journalviewer.h"

#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <gen/files.h>
#include <journals/measjournal.h>
#include <journals/sysjournal.h>
#include <journals/workjournal.h>
#include <s2/s2util.h>
#include <widgets/epopup.h>
#include <widgets/wd_func.h>
#include <xml/xmlparser/xmlmoduleparser.h>

namespace journals
{

JournalViewer::JournalViewer(const QString &filepath, QWidget *parent) : QDialog(parent, Qt::Dialog)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle("Journal Viewer: " + filepath.mid(filepath.lastIndexOf('/') + 1));
    QByteArray fileData;
    if (Files::LoadFromFile(filepath, fileData) == Error::Msg::NoError)
    {
        S2::S2BFile s2bFile;
        S2Util util;
        auto result = util.parseS2B(fileData, s2bFile);
        switch (result)
        {
        case Error::Msg::NoError:
            showJournal(s2bFile);
            break;
        case Error::Msg::SizeError:
            QMessageBox::critical(this, "Ошибка", "Ошибка размера файла журнала");
            break;
        case Error::Msg::WrongFormatError:
            QMessageBox::critical(this, "Ошибка",
                "Неверный формат заголовка журнала\n"
                "Файл не является журналом в формате S2B");
            break;
        case Error::Msg::CrcError:
            QMessageBox::critical(this, "Ошибка", "Ошибка контрольной суммы");
            break;
        default:
            break;
        }
    }
    else
        QMessageBox::critical(this, "Ошибка", "Ошибка открытия файла журнала");
}

void JournalViewer::showJournal(const S2::S2BFile &file)
{
    auto type = JournalType(file.header.fname);
    auto jour = createJournal(type, file.header.typeB, file.header.typeM);
    if (jour)
    {
        m_journal.reset(jour);
        setupUI(file);
        showMaximized();
    }
}

BaseJournal *JournalViewer::createJournal(const JournalType type, const quint16 typeB, const quint16 typeM)
{
    BaseJournal *retJournal = nullptr;
    switch (type)
    {
    case JournalType::System:
        setWindowTitle("[SYSTEM JOURNAL] " + windowTitle());
        retJournal = new SysJournal(this);
        break;
    case JournalType::Work:
        setWindowTitle("[WORK JOURNAL] " + windowTitle());
        parseSettings(typeB, typeM);
        retJournal = new WorkJournal(m_workSettings, this);
        break;
    case JournalType::Meas:
        setWindowTitle("[MEAS JOURNAL] " + windowTitle());
        parseSettings(typeB, typeM);
        retJournal = new MeasJournal(m_measSettings, this);
        break;
    default:
        QMessageBox::critical(this, "Ошибка", "Получен журнал неизвестного формата");
        break;
    }
    return retJournal;
}

void JournalViewer::parseSettings(const quint16 typeB, const quint16 typeM)
{
    auto moduleParser = new Xml::ModuleParser(this);
    QObject::connect(moduleParser, &Xml::ModuleParser::workJourDataSending, this, &JournalViewer::workDataReceived);
    QObject::connect(moduleParser, &Xml::ModuleParser::measJourDataSending, this, &JournalViewer::measDataReceived);
    moduleParser->parse(typeB, typeM, { "journals" });
    moduleParser->deleteLater();
}

void JournalViewer::workDataReceived(const u32 id, const QString &desc)
{
    m_workSettings.insert(id, desc);
}

void JournalViewer::measDataReceived(const u32 index, const QString &header, //
    const journals::BinaryType type, bool visib)
{
    using MeasJournal = Device::XmlDataTypes::MeasJournal;
    m_measSettings.push_back(MeasJournal { index, type, visib, header });
}

void JournalViewer::saveExcelJournal()
{
    auto suggestedFilename = m_journal->getSuggestedFilename();
    if (!suggestedFilename.isEmpty())
    {
        auto filename = WDFunc::ChooseFileForSave(this, "Excel documents (*.xlsx)", "xlsx", suggestedFilename);
        if (!filename.isEmpty())
        {
            m_journal->saveToExcel(filename);
            EMessageBox::information(this, "Записано успешно!");
        }
    }
}

void JournalViewer::setupUI(const S2::S2BFile &file)
{
    auto layout = new QVBoxLayout;
    auto modelView = m_journal->createModelView(this);
    layout->addWidget(modelView);

    auto saveExcelButton = new QPushButton("Сохранить журнал в Excel файл", this);
    connect(saveExcelButton, &QPushButton::clicked, this, &JournalViewer::saveExcelJournal);
    layout->addWidget(saveExcelButton);

    auto closeButton = new QPushButton("Закрыть", this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::close);
    layout->addWidget(closeButton);

    setLayout(layout);
    m_journal->fill(file);
}

}
