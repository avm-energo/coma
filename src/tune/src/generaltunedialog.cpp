#include "tune/generaltunedialog.h"

#include <gen/files.h>
#include <gen/stdfunc.h>
#include <tune/tunereporter.h>
#include <tune/tunesequencefile.h>
#include <widgets/epopup.h>
#include <widgets/filefunc.h>
#include <widgets/hexpbfunc.h>

#include <QHBoxLayout>
#include <QIcon>
#include <QPainter>
#include <QPushButton>
#include <QSvgRenderer>

GeneralTuneDialog::GeneralTuneDialog(Device::CurrentDevice *device, QWidget *parent)
    : UDialog(device, parent)
    , m_reporter(new TuneReporter(this))
{
    m_tuneTabWidget = new TuneTabWidget;
    TuneSequenceFile::init(m_device->getUID());
    m_tuneStepCount = 0;
    m_tuneStartStep = 1;
}

void GeneralTuneDialog::SetupUI(bool noReport)
{
    QHBoxLayout *hlyout = new QHBoxLayout;
    QVBoxLayout *lyout = new QVBoxLayout;
    lyout->addStretch(100);
    u8 count = 1;
    // u8 startStep = m_tuneStartStep;
    for (auto &d : m_dialogList)
    {
        QString tns = "tn" + QString::number(count++);
        lyout->addWidget(HexPBFunc::NewHexagonPB(
            this, "tn" + QString::number(m_tuneStartStep++), [&d]() { d.dialog->show(); }, ":/tunes/" + tns + ".svg",
            d.caption));
    }
    if (!noReport)
    {
        lyout->addWidget(HexPBFunc::NewHexagonPB(
            this, "tnprotocol", [this]() { generateReport(); }, ":/tunes/tnprotocol.svg",
            "Генерация протокола регулировки"));
        ++m_tuneStepCount; // +1 на протокол регулировки
    }
    lyout->addStretch(100);
    hlyout->addLayout(lyout);
    hlyout->addWidget(m_tuneTabWidget->set(), 100);
    setLayout(hlyout);
    setCalibrButtons();
    disableSuccessMessage();
}

void GeneralTuneDialog::prepareReport() { }

int GeneralTuneDialog::addWidgetToTabWidget(QWidget *w, const QString &caption)
{
    return m_tuneTabWidget->addTabWidget(w, caption);
}

void GeneralTuneDialog::addTuneDialog(const TuneDialogStruct &dlgStruct)
{
    ++m_tuneStepCount;
    dlgStruct.dialog->setTuneStep(m_tuneStepCount);
    m_dialogList += dlgStruct;
}

u8 GeneralTuneDialog::getTuneStepsCount()
{
    return m_tuneStepCount;
}

void GeneralTuneDialog::setCalibrButtons()
{
    int calibrstep = TuneSequenceFile::value("step", "1").toInt();
    for (int i = 1; i < calibrstep; ++i)
        HexPBFunc::setHexagonPBProcessed(this, "tn" + QString::number(i));
    HexPBFunc::setHexagonPBNormal(this, "tn" + QString::number(calibrstep));
    for (int i = (calibrstep + 1); i <= m_tuneStepCount; ++i)
        HexPBFunc::setHexagonPBRestricted(this, "tn" + QString::number(i));
    if (calibrstep < m_tuneStepCount)
        HexPBFunc::setHexagonPBRestricted(this, "tnprotocol");
}

void GeneralTuneDialog::generateReport()
{
    if (EMessageBox::question(this, "Сохранить протокол поверки?"))
    {
        QString filename = FileFunc::ChooseFileForSave(this, "*.pdf", "pdf", getFilenameForDevice());
        if (!filename.isEmpty())
        {
            prepareReport();
            m_reporter->setupReportData(AbstractTuneDialog::getReportData());
            m_reporter->saveToFile(filename);
            EMessageBox::information(this, "Записано успешно!");
        }
        else
            EMessageBox::warning(this, "Действие отменено");
    }
}
