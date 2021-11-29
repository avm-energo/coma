#pragma once

#include "abstractcheckdialog.h"

class Check3133Dialog : public CheckDialog
{
    Q_OBJECT

public:
    Check3133Dialog(const CheckItem &item, const categoryMap &categories, QWidget *parent = nullptr);
    void setupUI() override;
    void updateBitStringData(const DataTypes::BitStringStruct &bs) override;

protected:
    auto inputQuantity() const
    {
        return std::get<check::detail::Record>(m_item.itemsVector.back()).count.value();
    }
    virtual UWidget *Bd1W() = 0;

    QColor m_normalColor = (Qt::green);
    QColor m_alarmColor = (Qt::red);
    uint32_t circleRadius = std::min(height(), width()) / 20;

private:
    void updatePixmap(bool isset, int position);
};

class CheckBase3133Dialog : public Check3133Dialog
{
public:
    CheckBase3133Dialog(const CheckItem &item, const categoryMap &categories, QWidget *parent = nullptr)
        : Check3133Dialog(item, categories, parent)
    {
    }

protected:
    UWidget *Bd1W() override;
};

class CheckMezz3133Dialog : public Check3133Dialog
{
public:
    CheckMezz3133Dialog(const CheckItem &item, const categoryMap &categories, QWidget *parent = nullptr)
        : Check3133Dialog(item, categories, parent)
    {
    }

protected:
    UWidget *Bd1W() override;
};
