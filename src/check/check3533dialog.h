#pragma once

#include "abstractcheckdialog.h"

constexpr uint16_t INPUT_QUANITY = 32;

class Check3533Dialog : public AbstractCheckDialog
{
    QColor m_normalColor = (Qt::green);
    QColor m_alarmColor = (Qt::red);
    uint32_t circleRadius = std::min(height(), width()) / 20;

public:
    Check3533Dialog(QWidget *parent = nullptr);

    void updatePixmap(bool isset, int position);
    void updateBitStringData(const DataTypes::BitStringStruct &bs) override;

private:
    UWidget *Bd1W();
};
