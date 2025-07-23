#pragma once

#include <gen/integers.h>
#include <oscillograms/trendview/trendviewmodel.h>

class TrendViewModelKIV final : public TrendViewModel
{
public:
    explicit TrendViewModelKIV(int pointsum);
    QStringList analogColors() const override;
    QStringList analogDescriptions() const override;
    QStringList analogValues() const override;
};

class TrendViewModelKIVOne final : public TrendViewModel
{
private:
    u32 m_id;

public:
    explicit TrendViewModelKIVOne(const u32 id, int pointsum);
    QStringList analogColors() const override;
    QStringList analogDescriptions() const override;
    QStringList analogValues() const override;
};
