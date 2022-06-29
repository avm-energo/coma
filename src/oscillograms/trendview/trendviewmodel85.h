#pragma once

#include "trendviewmodel.h"

class TrendViewModel85 : public TrendViewModel
{
public:
    TrendViewModel85(int pointsnum);
    virtual ~TrendViewModel85() override = default;

    QStringList analogColors() const override;
    QStringList digitalColors() const override;
    QStringList analogDescriptions() const override;
    QStringList digitalDescriptions() const override;
    QStringList analogValues() const override;
    QStringList digitalValues() const override;
};
