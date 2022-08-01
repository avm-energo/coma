#pragma once
#include "../widgets/udialog.h"

#include <set>

//#define ANIMATED_PLOT

class QCustomPlot;
class QCPPolarGraph;
class QCPItemText;
class QCPPolarAxisAngular;

struct Phase
{
    float U = 0;
    float I = 0;
    bool isU(quint32 reg) const
    {
        switch (reg)
        {
        case 2401:
        case 2402:
        case 2403:
            return true;
        default:
            return false;
        }
    }
    bool isI(quint32 reg) const
    {
        switch (reg)
        {
        case 2404:
        case 2405:
        case 2406:
            return true;
        default:
            return false;
        }
    }
    bool isValid() const
    {
        return !((U == 0) && (I == 0));
    }
    QVector<double> toVector() const
    {
        if (U < I)
        {
            if (U < 0)
                return { 360 + U, 0, I };
            else
                return { 360 - U, 0, I };
        }
        else
            return { U, 0, I };
        ;
    }
};

struct GraphData
{
    QCPPolarGraph *graph;
    std::set<quint32> regs;
    Phase phase;
    bool contains(quint32 reg)
    {
        auto search = regs.find(reg);
        if (search != regs.end())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};

class PlotDialog : public UDialog
{
public:
    PlotDialog(QWidget *parent = nullptr);

    void updateFloatData(const DataTypes::FloatStruct &fl) override;
    void reqUpdate() override;
    void setupUI();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    GraphData graphPhaseA, graphPhaseB, graphPhaseC;
    QCustomPlot *customPlot;
    QCPPolarAxisAngular *exampleAngularAxis = nullptr;

    void updateGraph(const GraphData &graphData, char phaseLiteral) const;
    QCPItemText *createText(QCustomPlot *parent, const QString &name, const QString &text) const;
    QCustomPlot *createExample();

#ifdef ANIMATED_PLOT
    int counter = 0;
#endif
};
