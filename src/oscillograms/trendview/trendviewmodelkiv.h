#pragma once

#include "trendviewmodel.h"

#include <gen/integers.h>

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
