#include <dialogs/slicegetdialog.h>
#include <engines/engines.h>
#include <gen/threadpool.h>
#include <widgets/emessagebox.h>
#include <widgets/filefunc.h>
#include <widgets/pbfunc.h>
#include <widgets/prbfunc.h>
#include <widgets/wdfunc.h>

#include <QLayout>

SliceGetDialog::SliceGetDialog(Device::CurrentDevice *device, QWidget *parent) : QDialog(parent), m_device(device)
{
    auto pool = Engines::Engines::GetInstance().getPool();
    connect(pool, &ThreadPool::finished, this, &SliceGetDialog::finished);
}

void SliceGetDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addStretch(100);
    hlyout->addWidget(PBFunc::New(this, "startpb", "Старт", this, &SliceGetDialog::startProcess));
    hlyout->addStretch(100);
    hlyout->addWidget(PBFunc::New(this, "closepb", "Закрыть",
        [this]()
        {
            emit cancel();
            this->close();
        }));
    hlyout->addStretch(100);
    hlyout->addWidget(PBFunc::New(this, "cancelpb", "Отмена",
        [this]()
        {
            emit cancel();
            WDFunc::SetEnabled(this, "startpb", true);
            WDFunc::SetEnabled(this, "cancelpb", false);
        }));
    lyout->addLayout(hlyout);
    lyout->addWidget(
        PrbFunc::NewLBL(this, "Получение блока Bsi", c_ProgressMap.value(Engines::Slices::Stages::BsiLoad)));
    lyout->addWidget(
        PrbFunc::NewLBL(this, "Получение блока BsiExt", c_ProgressMap.value(Engines::Slices::Stages::BsiLoadExt)));
    lyout->addWidget(
        PrbFunc::NewLBL(this, "Получение конфигурации", c_ProgressMap.value(Engines::Slices::Stages::ConfigLoad)));
    lyout->addWidget(PrbFunc::NewLBL(
        this, "Скачивание системного журнала", c_ProgressMap.value(Engines::Slices::Stages::SysJourLoad), "%v из %m"));
    lyout->addWidget(PrbFunc::NewLBL(
        this, "Скачивание журнала событий", c_ProgressMap.value(Engines::Slices::Stages::WorkJourLoad), "%v из %m"));
    lyout->addWidget(PrbFunc::NewLBL(
        this, "Скачивание журнала измерений", c_ProgressMap.value(Engines::Slices::Stages::MeasJourLoad), "%v из %m"));
    lyout->addWidget(PrbFunc::NewLBL(
        this, "Получение блока начальных значений", c_ProgressMap.value(Engines::Slices::Stages::StartupLoad)));
    lyout->addWidget(PrbFunc::NewLBL(
        this, "Получение блока настроечных параметров", c_ProgressMap.value(Engines::Slices::Stages::TuneLoad)));
    lyout->addWidget(PrbFunc::NewLBL(
        this, "Скачивание осциллограмм", c_ProgressMap.value(Engines::Slices::Stages::GetOsc), "%v из %m"));
    lyout->addWidget(PrbFunc::NewLBL(
        this, "Получение текущих измерений", c_ProgressMap.value(Engines::Slices::Stages::GetCurrentState)));
    lyout->addWidget(
        PrbFunc::NewLBL(this, "Сохранение результатов", c_ProgressMap.value(Engines::Slices::Stages::Save)));
    setLayout(lyout);
    WDFunc::SetEnabled(this, "cancelpb", false);
}

void SliceGetDialog::setRange(Engines::Slices::Stages stage, qint64 max)
{
    PrbFunc::setRange(this, c_ProgressMap.value(stage), max);
}

void SliceGetDialog::setPrbValue(Engines::Slices::Stages stage, qint64 value)
{
    PrbFunc::setValue(this, c_ProgressMap.value(stage), value);
}

void SliceGetDialog::startProcess()
{
    auto filename = FileFunc::ChooseFileForSave(this, "Zip files (*.zip)", "zip",
        QString("slice_%1").arg(QDateTime::currentDateTime().toString("yyMMdd_hhmmss")));
    if (filename.isEmpty())
        return;
    WDFunc::SetEnabled(this, "startpb", false);
    WDFunc::SetEnabled(this, "cancelpb", true);
    auto engine = new Engines::Slices(m_device, filename);
    connect(engine, &Engines::Slices::setProgressRange, this, &SliceGetDialog::setRange);
    connect(engine, &Engines::Slices::setProgressValue, this, &SliceGetDialog::setPrbValue);
    connect(engine, &Engines::Slices::result, this, &SliceGetDialog::sliceResultReceived);
    connect(this, &SliceGetDialog::cancel, engine, &Engines::Slices::cancel, Qt::DirectConnection);
    clearPrbs();
    isCancelled = false;
    auto pool = Engines::Engines::GetInstance().getPool();
    m_proc_id = pool->create(engine, &Engines::Slices::createSlice, &Engines::Slices::finished);
    pool->start(m_proc_id);
}

void SliceGetDialog::finished(int id)
{
    if (id == m_proc_id)
    {
        WDFunc::SetEnabled(this, "startpb", true);
        WDFunc::SetEnabled(this, "cancelpb", false);
        if (!isCancelled)
            EMessageBox::information(this, "Снимок создан");
        else
            EMessageBox::warning(this, "Отмена");
    }
}

void SliceGetDialog::sliceResultReceived(Error::Msg result)
{
    if (result != Error::Msg::NoError)
    {
        clearPrbs();
        isCancelled = true;
    }
}

void SliceGetDialog::clearPrbs()
{
    for (auto stage : c_ProgressMap.keys())
    {
        setPrbValue(stage, 0);
    }
}
