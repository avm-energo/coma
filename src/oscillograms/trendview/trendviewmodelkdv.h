#pragma once

#include "trendviewmodel.h"

class TrendViewModelKDV : public TrendViewModel
{
public:
    TrendViewModelKDV(int pointsnum);
    virtual ~TrendViewModelKDV() override = default;

    QStringList analogColors() const override;
    QStringList analogDescriptions() const override;
    QStringList analogValues() const override;
    QString xAxisDescription() const override;
    void setAnalogData(const QString &name, QVector<double> &data);
};

class TrendViewModelKDVSpec : public TrendViewModel
{
public:
    TrendViewModelKDVSpec(int pointsnum);
    virtual ~TrendViewModelKDVSpec() override = default;

    QStringList analogColors() const override;
    QStringList analogDescriptions() const override;
    QStringList analogValues() const override;
    QString xAxisDescription() const override;
    void setAnalogData(const QString &name, QVector<double> &data);
};
