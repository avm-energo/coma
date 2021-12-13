#ifndef TRENDVIEWDIALOG_H
#define TRENDVIEWDIALOG_H

#include "trendviewmodel.h"

#include <../qcustomplot/qcustomplot.h>
#include <QDialog>

constexpr int MAXGRAPHSPERPLOT = 14;

class QToolBar;
class SignalChooseWidget;

class TrendViewDialog : public QDialog
{
    Q_OBJECT

public:
    enum SignalTypes : bool
    {
        ST_ANALOG,
        ST_DIGITAL
    };

    struct Size
    {
        float min;
        float max;
    };

    TrendViewDialog(QWidget *parent = nullptr);

    // инициализация графиков
    // имена графиков контактных/аналоговых сигналов, количество точек, диапазон по оси Y для аналоговых
    void showPlot();
    void setModel(TrendViewModel *model);
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

    void setTrendModel(TrendViewModel *mdl);

private:
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

    struct Signals
    {
        DescriptionStruct description;
        QCPLegend *legend;
        // should we rescale upper and lower ranges automatically to let the zero not moving
        bool rescaleActivated;
        SignalTypes type;
        bool noSignals() const
        {
            return description.names.empty();
        }
    };

    TrendViewModel *m_trendModel;
    QMap<QString, SignalOscPropertiesStruct> signalOscPropertiesMap;
    std::unique_ptr<QCustomPlot> mainPlot;

    Signals analog, digital;
    Size sizeX, sizeY;

    bool rangeChangeInProgress, starting;
    bool rangeAxisInProgress, startingAx;

    QToolBar *createToolBar(SignalTypes type);
    void changeRange(const QCPRange &range);
    QCPLegend *createLegend(int rectindex);
    /// get current visible signals of type type
    ptrdiff_t visibleSignalOscDescriptionSize(SignalTypes type) const;

    void analogAxis(int &MainPlotLayoutRow);
    void analogAxisDefault(int graphNum, QCPAxisRect *axisRect);
    void analogAxis85(int graphNum, QCPAxisRect *axisRect);
    void digitalAxis(int &MainPlotLayoutRow);

    void showAxes(QCPGraph *graph, const QVector<double> &keys, const QVector<double> &values, TrendViewDialog::SignalTypes signalType, QString &widgetName);
    SignalChooseWidget *setupHelper(const TrendViewDialog::Signals &sig);

    void removeSig(QString signame);
    void addSig(QString signame);

    Q_ENUM(SignalTypes)
private slots:
    void graphClicked(QCPAbstractPlottable *plot, int dataIndex);
    void signalChoosed(QString signame) const;
    void signalToggled(QString signame, bool isChecked);
    void digitalRangeChanged(const QCPRange &range);
    void analogRangeChanged(const QCPRange &range);
    void mouseWheel();

    void setRescale(bool isChecked);
    void autoResizeRange(QCPAxisRect *rect, int index) const;
};

#endif // TRENDVIEWDIALOG_H
