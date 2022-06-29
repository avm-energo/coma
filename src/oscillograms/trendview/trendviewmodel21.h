#pragma once

#include "trendviewmodel.h"

class TrendViewModel21 : public TrendViewModel
{
public:
    TrendViewModel21(int pointsnum);
    virtual ~TrendViewModel21() override = default;

    QStringList analogColors() const override;
    QStringList digitalColors() const override;
    QStringList analogDescriptions() const override;
    QStringList digitalDescriptions() const override;
    QStringList analogValues() const override;
    QStringList digitalValues() const override;
    void addAnalogValue(const QString &value) override;

private:
    QStringList m_analogValues;
};
