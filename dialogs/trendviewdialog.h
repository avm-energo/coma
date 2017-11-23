#ifndef TRENDVIEWDIALOG_H
#define TRENDVIEWDIALOG_H

#include <QDialog>
#include <QPoint>
#include <QToolBar>
#include "../gen/qcustomplot.h"

class TrendViewDialog : public QDialog
{
    Q_OBJECT
public:
    TrendViewDialog(QVector<QString> &DigitalTrendNames, QVector<QString> &AnalogTrendNames, int PointNum, \
                    float XRangeMin, float XRangeMax, float YRangeMin, float YRangeMax, QWidget *parent=0);
    ~TrendViewDialog();

     // инициализация графиков
    // имена графиков контактных/аналоговых сигналов, количество точек, диапазон по оси Y для аналоговых
    void AddAnalogPoint(int GraphNum, float PointValue);
    void AddDigitalPoint(int GraphNum, int PointValue);
    void PlotShow();
    bool SetPointsAxis(float start, float step);
    void SetFilename(const QString &fn);

private:
    QCustomPlot *AnalogPlot, *DiscretePlot;
    QVector<QCPGraph *> AnalogGraphs, DigitalGraphs;
    QVector<QVector<double> > AnalogMainData, DigitalMainData;
    QVector<double> MainPoints;
    int PointsNum;
    QString Filename;
    bool NoDiscrete, NoAnalog;

    void SetupUI();
    QToolBar *PlotToolBar();

private slots:
    void SaveToExcel();
    void SaveToComtrade();
    void AnalogPlotContextMenu(QPoint pos);
    void DiscretePlotContextMenu(QPoint pos);
    void MoveLegend();
    void ChooseGraphsToDisplay();
};

#endif // TRENDVIEWDIALOG_H
