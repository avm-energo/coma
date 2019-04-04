#ifndef TRENDVIEWDIALOG_H
#define TRENDVIEWDIALOG_H

#include <QDialog>
#include <QPoint>
#include <QToolBar>
#include "../models/trendviewmodel.h"
#include "../gen/qcustomplot.h"
#include "../gen/eoscillogram.h"


#define MAXGRAPHSPERPLOT    16
#define MT_ID85            10030 // ID осциллограммы по модулю 8085
#define ID_OSC_CH0         10040 // ID осциллограммы канала 0 модуля 87
#define ID_OSC_CH7         10047 // ID осциллограммы канала 7 модуля 87
#define MT_ID21         10001 // первый ID осциллограммы по модулю 21
#define MT_ID21E        10016 // последний ID осциллограммы по модулю 21
#define MT_ID80         10020 // ID осциллограммы по модулю 808x
#define MT_ID85         10030 // ID осциллограммы по модулю 8085

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
    void SetAnalogDescriptions(QStringList &descr);
    void SetDiscreteDescriptions(QStringList &descr);
    int WRow;
//    quint32 id;

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
    TrendViewModel *TrendModel;


private:
    enum SignalTypes
    {
        ST_ANALOG,
        ST_DIGITAL
    };

    struct DescriptionStruct
    {
        QStringList Names;
        QStringList Descriptions;
        QMap<QString, QString> Colors;
    };

    struct SignalOscPropertiesStruct
    {
        SignalTypes Type;
        int LeftAxisIndex;
        QCPGraph *Graph;
        bool Visible;
    };

    QMap<QString, SignalOscPropertiesStruct> SignalOscPropertiesMap;
    QPointer<QCustomPlot> MainPlot;
    QCPLegend *AnalogLegend, *DiscreteLegend;
//    QMap<QString, QCPGraph *> AnalogGraphs, DigitalGraphs;
    DescriptionStruct AnalogDescription, DigitalDescription;
    float XMin, XMax, YMin, YMax;
    bool NoDiscrete, NoAnalog;
    bool RangeChangeInProgress, Starting;
    bool RangeAxisInProgress, StartingAx;
    bool DigitalRescaleActivated, AnalogRescaleActivated; // should we rescale upper and lower ranges automatically
                            // to let the zero not moving
    QByteArray BAToSave;

    QToolBar *PlotToolBar(SignalTypes type);
    void ChangeRange(QCPRange range);
    QCPLegend *SetLegend(int rectindex);
    int SignalOscDescriptionSize(SignalTypes type); // get current visible signals of type type

private slots:
    void graphClicked(QCPAbstractPlottable *plot, int dataIndex);
    void SignalChoosed(QString signame);
    void SignalToggled(QString signame, bool isChecked);
    void DigitalRangeChanged(QCPRange range);
    void AnalogRangeChanged(QCPRange range);
    void MouseWheel();
    void MousePress();
    void SaveToExcel();
    void SaveToComtrade();
    void SaveToOsc();
    void SetRescale(bool isChecked);
    void AutoResizeRange(QCPAxisRect *rect, int index);
};

#endif // TRENDVIEWDIALOG_H
