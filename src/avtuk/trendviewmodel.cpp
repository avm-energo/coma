#include "trendviewmodel.h"

TrendViewModel::TrendViewModel(const QStringList &dlist, const QStringList &alist, int pointsnum)

{
    PointsNum = pointsnum;
    DigitalNames = dlist;
    AnalogNames = alist;
    for (int i = 0; i < dlist.size(); ++i)
        DigitalMainData.insert(dlist.at(i), QVector<double>());
    for (int i = 0; i < alist.size(); ++i)
        AnalogMainData.insert(alist.at(i), QVector<double>());
}

void TrendViewModel::AddAnalogPoint(const QString &GraphNum, float PointValue)
{
    QVector<double> tmpv = AnalogMainData[GraphNum];
    tmpv.append(PointValue);
    AnalogMainData[GraphNum] = tmpv;
}

void TrendViewModel::AddDigitalPoint(const QString &GraphNum, int PointValue)
{
    if (DigitalMainData.contains(GraphNum))
    {
        QVector<double> tmpv = DigitalMainData[GraphNum];
        double value = static_cast<double>(PointValue) + static_cast<double>(DigitalNames.indexOf(GraphNum)) * 1.5f;
        tmpv.append(value);
        DigitalMainData[GraphNum] = tmpv;
    }
}

bool TrendViewModel::SetPointsAxis(float start, float step)
{
    if (step <= 0)
    {
        qWarning("Ошибка в принятой осциллограмме, шаг отрицательный или равен нулю");
        return false;
    }
    if (PointsNum == 0)
        return false;
    MainPoints.clear();
    for (int i = 0; i < PointsNum; ++i)
    {
        MainPoints << start;
        start += step;
    }
    return true;
}

int TrendViewModel::DContains(const QString &key)
{
    return DigitalMainData.contains(key);
}

int TrendViewModel::AContains(const QString &key)
{
    return AnalogMainData.contains(key);
}

void TrendViewModel::SaveID(quint32 id)
{
    idOsc = id;
}

void TrendViewModel::processDigitalNames(const QStringList &list)
{
    DigitalNames = list;
    for (auto &&item : list)
        DigitalMainData.insert(item, QVector<double>());
}

void TrendViewModel::processAnalogNames(const QStringList &list)
{
    AnalogNames = list;
    for (auto &&item : list)
        AnalogMainData.insert(item, QVector<double>());
}
