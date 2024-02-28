#pragma once

#include "verification_offset.h"

#include <QTimer>
#include <QWidget>

class Mip;

class Tune82IoWidget final : public QWidget
{
    Q_OBJECT
private:
    QTimer *m_timer;
    Bd182 *m_bd1;
    Mip *m_mip;
    VerificationOffset m_calcValues;

    void setupUI();
    void updateData();
    void updateUI(const MipDataStruct &mipData);
    void updateUI(const Bd182::BlockData &devData);

public:
    explicit Tune82IoWidget(Modules::MezzanineBoard type, QWidget *parent = nullptr);
};
