#include "trendviewmodel.h"

TrendViewModel::TrendViewModel(int pointsnum) : pointsNum(pointsnum)
{
}

void TrendViewModel::addAnalogPoint(const QString &graphNum, double pointValue)
{
    if (m_analogMainData.contains(graphNum))
    {
        QVector<double> tmpv = m_analogMainData.value(graphNum);
        tmpv.append(pointValue);
        m_analogMainData.insert(graphNum, tmpv);
    }
}

void TrendViewModel::addDigitalPoint(const QString &graphNum, int pointValue)
{
    if (m_digitalMainData.contains(graphNum))
    {
        QVector<double> tmpv = m_digitalMainData.value(graphNum);
        double value = static_cast<double>(pointValue) + static_cast<double>(digitalNames.indexOf(graphNum)) * 1.5;
        tmpv.append(value);
        m_digitalMainData.insert(graphNum, tmpv);
    }
}

bool TrendViewModel::setPointsAxis(float start, float step)
{
    if (step <= 0)
    {
        qWarning("Ошибка в принятой осциллограмме, шаг отрицательный или равен нулю");
        return false;
    }
    if (pointsNum == 0)
        return false;
    m_mainPoints.clear();
    for (int i = 0; i < pointsNum; ++i)
    {
        m_mainPoints << start;
        start += step;
    }
    return true;
}

int TrendViewModel::digitalContains(const QString &key) const
{
    return m_digitalMainData.contains(key);
}

int TrendViewModel::analogContains(const QString &key) const
{
    return m_analogMainData.contains(key);
}

void TrendViewModel::processDigitalNames(const QStringList &list)
{
    digitalNames = list;
    for (auto &&item : list)
        m_digitalMainData.insert(item, QVector<double>());
}

void TrendViewModel::processAnalogNames(const QStringList &list)
{
    analogNames = list;
    for (auto &&item : list)
        m_analogMainData.insert(item, QVector<double>());
}

float TrendViewModel::xmin() const
{
    return m_xmin;
}

void TrendViewModel::setXmin(float newXmin)
{
    m_xmin = newXmin;
}

float TrendViewModel::xmax() const
{
    return m_xmax;
}

void TrendViewModel::setXmax(float newXmax)
{
    m_xmax = newXmax;
}

quint32 TrendViewModel::length() const
{
    return m_length;
}

void TrendViewModel::setLength(quint32 newLength)
{
    m_length = newLength;
}

quint32 TrendViewModel::idOsc() const
{
    return m_idOsc;
}

void TrendViewModel::setIdOsc(quint32 newIdOsc)
{
    m_idOsc = newIdOsc;
}

const QVector<double> &TrendViewModel::mainPoints() const
{
    return m_mainPoints;
}

void TrendViewModel::setMainPoints(const QVector<double> &newMainPoints)
{
    m_mainPoints = newMainPoints;
}

const QMap<QString, QVector<double>> &TrendViewModel::analogMainData() const
{
    return m_analogMainData;
}

void TrendViewModel::setAnalogMainData(const QMap<QString, QVector<double>> &newAnalogMainData)
{
    m_analogMainData = newAnalogMainData;
}

const QMap<QString, QVector<double>> &TrendViewModel::digitalMainData() const
{
    return m_digitalMainData;
}

void TrendViewModel::setDigitalMainData(const QMap<QString, QVector<double>> &newDigitalMainData)
{
    m_digitalMainData = newDigitalMainData;
}
