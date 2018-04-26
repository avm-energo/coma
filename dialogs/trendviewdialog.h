#ifndef TRENDVIEWDIALOG_H
#define TRENDVIEWDIALOG_H

#include <QDialog>
#include <QPoint>
#include <QToolBar>
#include "../models/trendviewmodel.h"
#include "../gen/qcustomplot.h"

class TrendViewDialog : public QDialog
{
    Q_OBJECT
public:
    TrendViewDialog(QByteArray &ba, QWidget *parent=0);
    ~TrendViewDialog();

    // инициализация графиков
    // имена графиков контактных/аналоговых сигналов, количество точек, диапазон по оси Y для аналоговых
    void PlotShow();
    void SetModel(TrendViewModel *model);
    void SetRanges(float XRangeMin, float XRangeMax, float YRangeMin, float YRangeMax);
    void SetDigitalNames(QStringList &names);
    void SetAnalogNames(QStringList &names);
    void SetupPlots();
    void SetupUI();

private:
    QCustomPlot *AnalogPlot, *DiscretePlot;
    QMap<QString, QCPGraph *> AnalogGraphs, DigitalGraphs;
    QStringList AnalogNames, DigitalNames;
    float XMin, XMax, YMin, YMax;
    bool NoDiscrete, NoAnalog;
    TrendViewModel *TrendModel;
    bool RangeChangeInProgress, Starting;
    QByteArray BAToSave;

    QToolBar *PlotToolBar();
    QCPGraph *GraphByName(QCustomPlot *plot, const QString &name);

private slots:
    void graphClicked(QCPAbstractPlottable *plot, int dataIndex);
    void ASignalChoosed(QString signame);
    void ASignalToggled(QString signame, bool isChecked);
    void DSignalChoosed(QString signame);
    void DSignalToggled(QString signame, bool isChecked);
    void GraphSetVisible(QCustomPlot *plot, const QString &graphname, bool visible);
    void DigitalRangeChanged(QCPRange range);
    void AnalogRangeChanged(QCPRange range);
    void SaveToExcel();
    void SaveToComtrade();
    void SaveToOsc();
};

#endif // TRENDVIEWDIALOG_H
