#pragma once

#include "abstractcheckdialog.h"

class Check3533Dialog : public CheckDialog
{
    Q_OBJECT
    QColor m_normalColor = (Qt::green);
    QColor m_alarmColor = (Qt::red);
    uint32_t circleRadius = std::min(height(), width()) / 20;

public:
    Check3533Dialog(const CheckItem &item, const categoryMap &categories, QWidget *parent = nullptr);
    void setupUI() override;
    void updatePixmap(bool isset, int position);
    void updateBitStringData(const DataTypes::BitStringStruct &bs) override;

private:
    UWidget *Bd1W();
};
