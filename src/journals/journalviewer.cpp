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
        auto s2bFormat = S2::parseS2B(fileData);
        auto type = JournalType(s2bFormat.file.ID);
        auto jour = createJournal(type, s2bFormat.header.typeB, s2bFormat.header.typeM);
        if (jour)
        {
            journal.reset(jour);
            setupUI(s2bFormat.file);
            showMaximized();
        }
    }
    else
    {
        EMessageBox::error(this, "Ошибка открытия файла журнала");
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
