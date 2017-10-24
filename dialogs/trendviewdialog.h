#ifndef TRENDVIEWDIALOG_H
#define TRENDVIEWDIALOG_H

#include <QDialog>
#include "../qcustomplot.h"

class TrendViewDialog : public QDialog
{
    Q_OBJECT
public:
    TrendViewDialog(QWidget *parent=0);
    ~TrendViewDialog();

     // инициализация графиков
    void Init(QVector<QString> &DigitalTrendNames, QVector<QString> &AnalogTrendNames, int PointNum, float RangeMin, float RangeMax);
    // имена графиков контактных/аналоговых сигналов, количество точек, диапазон по оси Y для аналоговых
    void AddAnalogPoint(int GraphNum, float PointValue);
    void AddDigitalPoint(int GraphNum, int PointValue);
    void PlotShow();
    void SetPointsAxis(float start, float step);

private:
    QCustomPlot *Plot;
    QVector<QCPGraph *> *AnalogGraphs, *DigitalGraphs;
    QVector<QVector<double> > AnalogMainData, DigitalMainData;
    QVector<double> MainPoints;
    int PointsNum;

    void SetupUI();
};

#endif // TRENDVIEWDIALOG_H
