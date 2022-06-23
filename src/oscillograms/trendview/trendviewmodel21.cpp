#include "trendviewmodel21.h"

#include "../gen/colors.h"

TrendViewModel21::TrendViewModel21(int pointsnum) : TrendViewModel(pointsnum)
{
}

QStringList TrendViewModel21::analogColors() const
{
    return { Colors::YLWCOLOR };
}

QStringList TrendViewModel21::digitalColors() const
{
    return {};
}

QStringList TrendViewModel21::analogDescriptions() const
{
    return { "Сигнал в инженерных единицах" };
}

QStringList TrendViewModel21::digitalDescriptions() const
{
    return {};
}

QStringList TrendViewModel21::analogValues() const
{
    return m_analogValues;
}

QStringList TrendViewModel21::digitalValues() const
{
    return {};
}

void TrendViewModel21::addAnalogValue(const QString &value)
{
    m_analogValues.append(value);
}
