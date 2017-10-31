#include <QVector>
#include <QPen>
#include "trendviewdialog.h"
#include "../publicclass.h"

TrendViewDialog::TrendViewDialog(QWidget *parent) : QDialog(parent)
{
    PointsNum = 0;
    QString tmps = "QDialog {background-color: "+QString(MAINWINCLRA1)+";}";
    setStyleSheet(tmps);
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

TrendViewDialog::~TrendViewDialog()
{

}

void TrendViewDialog::Init(QVector<QString> &DigitalTrendNames, QVector<QString> &AnalogTrendNames, int PointNum, float RangeMin, float RangeMax)
{
    QPen pen;
    int row = 0;
    this->PointsNum = PointNum;
    int DigitalGraphNum = DigitalTrendNames.size();
    int AnalogGraphNum = AnalogTrendNames.size();
    Plot->plotLayout()->clear();
    Plot->legend = new QCPLegend;
    Plot->legend->setLayer("legend");
    Plot->legend->setVisible(true);
    Plot->legend->setFont(QFont("Helvetica", 8));
    Plot->setAutoAddPlottableToLegend(true);
    Plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    if (DigitalGraphNum != 0)
    {
        QCPAxisRect *DigitalAxisRect = new QCPAxisRect(Plot);
//        DigitalAxisRect->addAxes(QCPAxis::atBottom | QCPAxis::atLeft);
        DigitalAxisRect->setBackground(QBrush(QColor(ACONFYCLR)));
        Plot->plotLayout()->addElement(row++, 0, DigitalAxisRect);
        DigitalMainData.resize(DigitalGraphNum);
        int count = 0;
        while (count < DigitalGraphNum)
        {
            QCPGraph *graph = Plot->addGraph(DigitalAxisRect->axis(QCPAxis::atBottom), DigitalAxisRect->axis(QCPAxis::atLeft));
            pen.setColor(QColor(qSin(count*1+1.2)*80+80, qSin(count*0.3+0)*80+80, qSin(count*0.3+1.5)*80+80));
            graph->setPen(pen);
            graph->valueAxis()->setRange(-1, 2);
            graph->keyAxis()->setLabel("Time, ns");
            graph->valueAxis()->setLabel(DigitalTrendNames.at(count));
            DigitalGraphs.append(graph);
/*            QVector<double> np;
            DigitalMainData.append(np); */
            ++count;
        }
        DigitalAxisRect->insetLayout()->addElement(Plot->legend, Qt::AlignRight|Qt::AlignTop);
        DigitalAxisRect->insetLayout()->setMargins(QMargins(12, 12, 12, 12));
    }
    if (AnalogGraphNum != 0)
    {
        QCPAxisRect *AnalogAxisRect = new QCPAxisRect(Plot);
//        AnalogAxisRect->addAxes(QCPAxis::atBottom | QCPAxis::atLeft);
        AnalogAxisRect->setBackground(QBrush(QColor(ACONFYCLR)));
        Plot->plotLayout()->addElement(row++, 0, AnalogAxisRect);
        AnalogMainData.resize(AnalogGraphNum);
        int count = 0;
        while (count < AnalogGraphNum)
        {
            QCPGraph *graph = Plot->addGraph(AnalogAxisRect->axis(QCPAxis::atBottom), AnalogAxisRect->axis(QCPAxis::atLeft));
            pen.setColor(QColor(qSin(count*1+1.2)*80+80, qSin(count*0.3+0)*80+80, qSin(count*0.3+1.5)*80+80));
            graph->setPen(pen);
            graph->valueAxis()->setRange(RangeMin, RangeMax);
            graph->keyAxis()->setLabel("Time, ns");
            graph->valueAxis()->setLabel(AnalogTrendNames.at(count));
            AnalogGraphs.append(graph);
/*            QVector<double> np;
            AnalogMainData.append(np); */
            ++count;
        }
        AnalogAxisRect->insetLayout()->addElement(Plot->legend, Qt::AlignRight|Qt::AlignTop);
        AnalogAxisRect->insetLayout()->setMargins(QMargins(12, 12, 12, 12));
    }
}

void TrendViewDialog::AddAnalogPoint(int GraphNum, float PointValue)
{
    if (GraphNum < AnalogMainData.size())
    {
        QVector<double> tmpv = AnalogMainData.at(GraphNum);
        tmpv.append(PointValue);
        AnalogMainData.replace(GraphNum, tmpv);
    }
}

void TrendViewDialog::AddDigitalPoint(int GraphNum, int PointValue)
{
    if (GraphNum < DigitalMainData.size())
    {
        QVector<double> tmpv = DigitalMainData.at(GraphNum);
        tmpv.append(PointValue);
        DigitalMainData.replace(GraphNum, tmpv);
    }
}

void TrendViewDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    Plot = new QCustomPlot;
    lyout->addWidget(Plot);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(close()));
    hlyout->addStretch(10);
    hlyout->addWidget(pb);
    hlyout->addStretch(10);
    lyout->addLayout(hlyout);
    setLayout(lyout);
}

void TrendViewDialog::PlotShow()
{
    int count = 0;
    while (count < AnalogGraphs.size())
    {
        if (count < AnalogMainData.size())
        {
            AnalogGraphs.at(count)->setData(MainPoints, AnalogMainData.at(count));
            AnalogGraphs.at(count)->rescaleKeyAxis();
            AnalogGraphs.at(count)->rescaleValueAxis();
        }
        ++count;
    }
    count = 0;
    while (count < DigitalGraphs.size())
    {
        if (count < DigitalMainData.size())
        {
            DigitalGraphs.at(count)->setData(MainPoints, DigitalMainData.at(count));
            DigitalGraphs.at(count)->rescaleKeyAxis();
            DigitalGraphs.at(count)->rescaleValueAxis();
        }
        ++count;
    }
    Plot->replot();
    this->showMaximized();
}

void TrendViewDialog::SetPointsAxis(float start, float step)
{
    if (PointsNum == 0)
        return;
    MainPoints.clear();
    for (int i=0; i<PointsNum; ++i)
    {
        MainPoints << start;
        start += step;
    }
}
