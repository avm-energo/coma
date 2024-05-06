#pragma once

#include <oscillograms/trendview/trendviewmodel.h>

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

class TrendViewModelKDVV : public TrendViewModelKDV
{
public:
    TrendViewModelKDVV(int pointsnum);
    virtual ~TrendViewModelKDVV() override = default;
    QStringList analogDescriptions() const override;
    QStringList analogValues() const override;
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

class TrendViewModelKDVVSpec : public TrendViewModelKDVSpec
{
public:
    TrendViewModelKDVVSpec(int pointsnum);
    virtual ~TrendViewModelKDVVSpec() override = default;
    QStringList analogDescriptions() const override;
    QStringList analogValues() const override;
};
