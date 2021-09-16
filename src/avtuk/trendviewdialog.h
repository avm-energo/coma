#ifndef TRENDVIEWDIALOG_H
#define TRENDVIEWDIALOG_H

#include "../qcustomplot/qcustomplot.h"
#include "trendviewmodel.h"

#include <QDialog>

constexpr int MAXGRAPHSPERPLOT = 14;

class QToolBar;

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
    void setDigitalNames(const QStringList &names);
    void setAnalogNames(const QStringList &names);
    void setDigitalColors(const QStringList &colors);
    void setAnalogColors(const QStringList &colors);
    void setupPlots();
    void setupUI();

    // блока данных
    void setAnalogDescriptions(const QStringList &descr);
    void setDiscreteDescriptions(const QStringList &descr);

    void setTrendModel(std::unique_ptr<TrendViewModel> mdl);

    QByteArray arrayToSave() const;
    void setArrayToSave(const QByteArray &arrayToSave);

private:
    std::unique_ptr<TrendViewModel> m_trendModel;
    QMap<QString, SignalOscPropertiesStruct> signalOscPropertiesMap;
    std::unique_ptr<QCustomPlot> mainPlot;
    QCPLegend *analogLegend, *discreteLegend;

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

    void saveToOsc();
    void setRescale(bool isChecked);
    void autoResizeRange(QCPAxisRect *rect, int index);
};

#endif // TRENDVIEWDIALOG_H
