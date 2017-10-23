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

    void Init(int GraphNum, int PointNum, float RangeMin, float RangeMax); // инициализация графиков
                                                                           // количество самих графиков, количество точек, диапазон по оси X
    void SetPoint(int GraphNum, float PointValue);
    void PlotShow();

private:
    QCustomPlot *Plot;
    QVector<QVector<double> > MainData;

    void SetupUI();
};

#endif // TRENDVIEWDIALOG_H
