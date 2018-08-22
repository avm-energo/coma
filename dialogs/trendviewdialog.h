#ifndef TRENDVIEWDIALOG_H
#define TRENDVIEWDIALOG_H

#include <QDialog>
#include <QPoint>
#include <QToolBar>
#include "../models/trendviewmodel.h"
#include "../gen/qcustomplot.h"
#include "../gen/eoscillogram.h"

#define MAXGRAPHSPERPLOT    16

class TrendViewDialog : public QDialog
{
    Q_OBJECT
public:
    TrendViewDialog(QByteArray &ba, QWidget *parent=nullptr);
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
    void WriteToFile(int row, QXlsx::Document *xls); // row - номер строки для записи в файл xlsx, bdnum - номер блока данных
    int WRow;

    struct Bd1
    {
        float Frequency;            // Частота в сети
        float IUefNat_filt[9];  	// Истинные действующие значения сигналов (в вольтах или амперах на входе)
        float UefNatLin_filt[6];	// Истинные действующие значения линейных напряжений 1-й и 2-й групп
        float PNatf[3];             // истинная активная мощность по фазам
        float QNatf[3];             // реактивная мощность по кажущейся полной и истинной активной
        float SNatf[3];             // кажущаяся полная мощность по эфф. токам и напряжениям
        float CosPhiNat[3];         // cos phi по истинной активной мощности, по фазам
        quint32 DD_in;              // дискретные входные данные
                                    // разряды 0..11 -состояние входов 220_DI0.. 220_DI11
                                    // разряды 12,13 - состояние входов 24_DI0, 24_DI1
    };
    Bd1 Bd_block1;

    struct Point85
    {
        float An[9];    // Ua,Ub,Uc (напряжение источника), Ia, Ib, Ic (ток ВВ), Ua,Ub,Uc (напряжение нагрузки)
        quint32 Dis;
    };

    Point85 point;

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
