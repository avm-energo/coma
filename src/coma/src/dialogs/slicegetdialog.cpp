#include <dialogs/slicegetdialog.h>

SliceGetDialog::SliceGetDialog(Device::CurrentDevice *device, QWidget *parent) : QDialog(parent)
{
    m_engine = new Engines::Slices(device);
    connect(m_engine, &Engines::Slices::setProgressRange, this, &SliceGetDialog::setRange);
    connect(m_engine, &Engines::Slices::setProgressValue, this, &SliceGetDialog::setPrbValue);
}

void SliceGetDialog::SetupUI() { }

void SliceGetDialog::setRange(Engines::Slices::Stages stage, qint64 max) { }

void SliceGetDialog::setPrbValue(Engines::Slices::Stages stage, qint64 value) { }
