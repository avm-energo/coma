#pragma once

#include <QTimer>
#include <QWidget>
#include <device/current_device.h>
#include <tune/82/verification_offset.h>

class Mip;

class Tune82IoWidget final : public QWidget
{
    Q_OBJECT
private:
    Device::MezzanineBoard m_typeM;
    QTimer *m_timer;
    Bd182 *m_bd1;
    Mip *m_mip;
    VerificationOffset m_calcValues;

    void setupUI();
    void updateData();
    void updateUI(const MipDataStruct &mipData);
    void updateUI(const Bd182::BlockData &devData);

public:
    explicit Tune82IoWidget(Device::CurrentDevice *device, QWidget *parent = nullptr);
};
