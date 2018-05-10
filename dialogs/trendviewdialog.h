#ifndef TRENDVIEWDIALOG_H
#define TRENDVIEWDIALOG_H

#include <QDialog>
#include <QPoint>
#include <QToolBar>
#include "../models/trendviewmodel.h"
#include "../gen/qcustomplot.h"

#define MAXGRAPHSPERPLOT    16

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
    void SetDigitalColors(QStringList &colors);
    void SetAnalogColors(QStringList &colors);
    void SetupPlots();
    void SetupUI();

private:
    struct AnalogDescriptionStruct
    {
        QStringList Names;
        QMap<QString, QString> Colors;
    };
    int AnalogRectIndex, DiscreteRectIndex;
    QPointer<QCustomPlot> MainPlot;
    QCPLegend *AnalogLegend, *DiscreteLegend;
    QMap<QString, QCPGraph *> AnalogGraphs, DigitalGraphs;
    AnalogDescriptionStruct AnalogDescription, DigitalDescription;
    float XMin, XMax, YMin, YMax;
    bool NoDiscrete, NoAnalog;
    TrendViewModel *TrendModel;
    bool RangeChangeInProgress, Starting;
    QByteArray BAToSave;

    QToolBar *PlotToolBar();
    QCPGraph *GraphByName(const QString &name);
    void ChangeRange(QCPRange range);
    QCPLegend *SetLegend(int rectindex);

private slots:
    void graphClicked(QCPAbstractPlottable *plot, int dataIndex);
    void ASignalChoosed(QString signame);
    void ASignalToggled(QString signame, bool isChecked);
    void DSignalChoosed(QString signame);
    void DSignalToggled(QString signame, bool isChecked);
    void GraphSetVisible(int rectindex, const QString &graphname, bool visible);
    void DigitalRangeChanged(QCPRange range);
    void AnalogRangeChanged(QCPRange range);
    void SaveToExcel();
    void SaveToComtrade();
    void SaveToOsc();
};

#endif // TRENDVIEWDIALOG_H
