#ifndef TRENDVIEWMODEL_H
#define TRENDVIEWMODEL_H

#include <QVector>

class TrendViewModel
{
public:
    TrendViewModel(int digsize, int ansize, int pointsnum);
    ~TrendViewModel();

    QVector<QVector<double> > AnalogMainData, DigitalMainData;
    QVector<double> MainPoints;

     // инициализация графиков
    // имена графиков контактных/аналоговых сигналов, количество точек, диапазон по оси Y для аналоговых
    void AddAnalogPoint(int GraphNum, float PointValue);
    void AddDigitalPoint(int GraphNum, int PointValue);
    bool SetPointsAxis(float start, float step);
    void SetFilename(const QString &fn);
    int DSize();
    int ASize();

private:
    int PointsNum;
    QString Filename;
    bool NoDiscrete, NoAnalog;

private slots:
    void SaveToExcel();
    void SaveToComtrade();
};

#endif // TRENDVIEWMODEL_H
