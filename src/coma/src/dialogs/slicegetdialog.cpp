#include <dialogs/slicegetdialog.h>
#include <widgets/pbfunc.h>
#include <widgets/prbfunc.h>

#include <QLayout>

SliceGetDialog::SliceGetDialog(Device::CurrentDevice *device, QWidget *parent) : QDialog(parent)
{
    m_engine = new Engines::Slices(device);
    connect(m_engine, &Engines::Slices::setProgressRange, this, &SliceGetDialog::setRange);
    connect(m_engine, &Engines::Slices::setProgressValue, this, &SliceGetDialog::setPrbValue);
}

void SliceGetDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addStretch(100);
    hlyout->addWidget(PBFunc::New(this, "", "Старт", this, &SliceGetDialog::startProcess));
    hlyout->addStretch(100);
    lyout->addLayout(hlyout);
    lyout->addWidget(
        PrbFunc::NewLBL(this, "Получение блока Bsi", c_ProgressMap.value(Engines::Slices::Stages::BsiLoad)));
    lyout->addWidget(
        PrbFunc::NewLBL(this, "Получение блока BsiExt", c_ProgressMap.value(Engines::Slices::Stages::BsiLoadExt)));
    lyout->addWidget(
        PrbFunc::NewLBL(this, "Получение конфигурации", c_ProgressMap.value(Engines::Slices::Stages::ConfigLoad)));
    lyout->addWidget(PrbFunc::NewLBL(
        this, "Скачивание журнала измерений", c_ProgressMap.value(Engines::Slices::Stages::MeasJourLoad)));
    lyout->addWidget(PrbFunc::NewLBL(
        this, "Скачивание системного журнала", c_ProgressMap.value(Engines::Slices::Stages::SysJourLoad)));
    lyout->addWidget(PrbFunc::NewLBL(
        this, "Скачивание журнала событий", c_ProgressMap.value(Engines::Slices::Stages::WorkJourLoad)));
    lyout->addWidget(PrbFunc::NewLBL(
        this, "Получение блока начальных значений", c_ProgressMap.value(Engines::Slices::Stages::StartupLoad)));
    lyout->addWidget(PrbFunc::NewLBL(
        this, "Получение блока настроечных параметров", c_ProgressMap.value(Engines::Slices::Stages::TuneLoad)));
    lyout->addWidget(
        PrbFunc::NewLBL(this, "Скачивание осциллограмм", c_ProgressMap.value(Engines::Slices::Stages::GetOsc)));
    lyout->addWidget(PrbFunc::NewLBL(
        this, "Получение текущих измерений", c_ProgressMap.value(Engines::Slices::Stages::GetCurrentState)));
    lyout->addWidget(
        PrbFunc::NewLBL(this, "Сохранение результатов", c_ProgressMap.value(Engines::Slices::Stages::Save)));
    hlyout->addStretch(100);
    hlyout->addWidget(PBFunc::New(this, "", "Закрыть", this, &SliceGetDialog::close));
    hlyout->addStretch(100);
    lyout->addLayout(hlyout);
    setLayout(lyout);
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
    m_engine->CreateSlice();
}
