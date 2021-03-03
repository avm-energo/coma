#ifndef TRENDVIEWDIALOG_H
#define TRENDVIEWDIALOG_H

#include "../qcustomplot/qcustomplot.h"
#include "eoscillogram.h"
#include "trendviewmodel.h"

#include <QDialog>
#include <QPoint>
#include <QToolBar>

constexpr int MAXGRAPHSPERPLOT = 14;
constexpr int MT_ID85 = 10030;  // ID осциллограммы по модулю 8085
constexpr int MT_ID21 = 10001;  // первый ID осциллограммы по модулю 21
constexpr int MT_ID21E = 10016; // последний ID осциллограммы по модулю 21
constexpr int MT_ID80 = 10020;  // ID осциллограммы по модулю 808x

class TrendViewDialog : public QDialog
{
    Q_OBJECT
public:
    enum SignalTypes
    {
        ST_ANALOG,
        ST_DIGITAL
    };

    struct DescriptionStruct
    {
        QStringList names;
        QStringList descriptions;
        QMap<QString, QString> colors;
    };

    struct SignalOscPropertiesStruct
    {
        SignalTypes type;
        int leftAxisIndex;
        QCPGraph *graph;
        bool isVisible;
    };
    TrendViewDialog(QByteArray &ba, QWidget *parent = nullptr);
    TrendViewDialog(QWidget *parent = nullptr);
    ~TrendViewDialog();

    // инициализация графиков
    // имена графиков контактных/аналоговых сигналов, количество точек, диапазон по оси Y для аналоговых
    void showPlot();
    void setModel(std::unique_ptr<TrendViewModel> model);
    void setRange(float XRangeMin, float XRangeMax, float YRangeMin, float YRangeMax);
    void setDigitalNames(QStringList &names);
    void setAnalogNames(QStringList &names);
    void setDigitalColors(QStringList &colors);
    void setAnalogColors(QStringList &colors);
    void setupPlots();
    void setupUI();
    // void WriteToFile(int row, QXlsx::Document *xls); // row - номер строки для записи в файл xlsx, bdnum - номер
    // блока данных
    void setAnalogDescriptions(QStringList &descr);
    void setDiscreteDescriptions(QStringList &descr);
    // int WRow;
    //    quint32 id;

    //    struct Point85
    //    {
    //        float An[9]; // Ua,Ub,Uc (напряжение источника), Ia, Ib, Ic (ток ВВ), Ua,Ub,Uc (напряжение нагрузки)
    //        quint32 Dis;
    //    };

    //    Point85 point;
    void setTrendModel(std::unique_ptr<TrendViewModel> mdl);

    QByteArray arrayToSave() const;
    void setArrayToSave(const QByteArray &arrayToSave);

private:
    std::unique_ptr<TrendViewModel> m_trendModel;
    QMap<QString, SignalOscPropertiesStruct> signalOscPropertiesMap;
    std::unique_ptr<QCustomPlot> mainPlot;
    QCPLegend *analogLegend, *discreteLegend;
    //    QMap<QString, QCPGraph *> AnalogGraphs, DigitalGraphs;
    DescriptionStruct analogDescription, digitalDescription;
    float xMin, xMax, yMin, yMax;
    bool noDiscrete, noAnalog;
    bool rangeChangeInProgress, starting;
    bool rangeAxisInProgress, startingAx;
    // should we rescale upper and lower ranges automatically to let the zero not moving
    bool digitalRescaleActivated, analogRescaleActivated;

    QByteArray m_arrayToSave;

    QToolBar *createToolBar(SignalTypes type);
    void changeRange(QCPRange range);
    QCPLegend *createLegend(int rectindex);
    int visibleSignalOscDescriptionSize(SignalTypes type); // get current visible signals of type type

    void analogAxis(int &MainPlotLayoutRow);

    void digitalAxis(int &MainPlotLayoutRow);

private slots:
    void graphClicked(QCPAbstractPlottable *plot, int dataIndex);
    void signalChoosed(QString signame);
    void signalToggled(QString signame, bool isChecked);
    void digitalRangeChanged(QCPRange range);
    void analogRangeChanged(QCPRange range);
    void mouseWheel();
    void mousePress();
    void saveToExcel();
    void saveToComtrade();
    void saveToOsc();
    void setRescale(bool isChecked);
    void autoResizeRange(QCPAxisRect *rect, int index);
};

#endif // TRENDVIEWDIALOG_H
