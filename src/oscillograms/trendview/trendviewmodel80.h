#pragma once

#include "trendviewmodel.h"

class TrendViewModel80 : public TrendViewModel
{
public:
    TrendViewModel80(int pointsnum);
    virtual ~TrendViewModel80() override = default;

    QStringList analogColors() const override;
    QStringList digitalColors() const override;
    QStringList analogDescriptions() const override;
    QStringList digitalDescriptions() const override;
    QStringList analogValues() const override;
    QStringList digitalValues() const override;
};
