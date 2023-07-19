#include "journalviewer.h"

#include "../s2/s2.h"
#include "../widgets/epopup.h"
#include "../xml/xmlparser/xmlmoduleparser.h"
#include "measjournal.h"
#include "sysjournal.h"
#include "workjournal.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <gen/files.h>

namespace journals
{

JournalViewer::JournalViewer(const QString &filepath, QWidget *parent) : QDialog(parent, Qt::Dialog)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowTitle("Journal Viewer: " + filepath.mid(filepath.lastIndexOf('/') + 1));
    QByteArray fileData;
    if (Files::LoadFromFile(filepath, fileData) == Error::Msg::NoError)
    {
        S2DataTypes::S2BFile s2bFile;
        auto result = S2::parseS2B(fileData, s2bFile);
        switch (result)
        {
        case Error::Msg::NoError:
            showJournal(s2bFile);
            break;
        case Error::Msg::SizeError:
            EMessageBox::error(this, "Ошибка размера файла журнала");
            break;
        case Error::Msg::WrongFormatError:
            EMessageBox::error(this,
                "Неверный формат заголовка журнала\n"
                "Файл не является журналом в формате S2B");
            break;
        case Error::Msg::CrcError:
            EMessageBox::error(this, "Ошибка контрольной суммы");
            break;
        default:
            break;
        }
    }
    else
        EMessageBox::error(this, "Ошибка открытия файла журнала");
}

void JournalViewer::showJournal(const S2DataTypes::S2BFile &s2bFile)
{
    auto type = JournalType(s2bFile.file.ID);
    auto jour = createJournal(type, s2bFile.header.typeB, s2bFile.header.typeM);
    if (jour)
    {
        journal.reset(jour);
        setupUI(s2bFile.file);
        showMaximized();
    }
}

BaseJournal *JournalViewer::createJournal(const JournalType type, const quint16 typeB, const quint16 typeM)
{
    BaseJournal *retJournal = nullptr;
    switch (type)
    {
    case JournalType::System:
        retJournal = new SysJournal(this);
        break;
    case JournalType::Work:
        parseSettings(typeB, typeM);
        retJournal = new WorkJournal(workSettings, this);
        break;
    case JournalType::Meas:
        parseSettings(typeB, typeM);
        retJournal = new MeasJournal(measSettings, this);
        break;
    default:
        EMessageBox::error(this, "Получен журнал неизвестного формата");
        break;
    }
    return retJournal;
}

void JournalViewer::parseSettings(const quint16 typeB, const quint16 typeM)
{
    auto moduleParser = new Xml::ModuleParser(typeB, typeM, false, this);
    QObject::connect(moduleParser, &Xml::ModuleParser::workJourDataSending, this, &JournalViewer::workDataReceived);
    QObject::connect(moduleParser, &Xml::ModuleParser::measJourDataSending, this, &JournalViewer::measDataReceived);
    moduleParser->parse({ "journals" });
    moduleParser->deleteLater();
}

void JournalViewer::workDataReceived(const quint32 id, const QString &desc)
{
    workSettings.insert(id, desc);
}

void JournalViewer::measDataReceived(const quint32 index, const QString &header, //
    const ModuleTypes::BinaryType type, bool visib)
{
    measSettings.append({ index, header, type, visib });
}

void JournalViewer::setupUI(const DataTypes::FileStruct &file)
{
    auto layout = new QVBoxLayout;
    auto modelView = journal->createModelView(this);
    layout->addWidget(modelView);
    setLayout(layout);
    journal->fill(file);
}

}
