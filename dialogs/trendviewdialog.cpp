#include <QVector>
#include <QPen>
#include <QAction>
#include "QtXlsx/xlsxdocument.h"
#include "../config/config.h"
#include "../widgets/signalchoosewidget.h"
#include "../widgets/wd_func.h"
#include "trendviewdialog.h"

TrendViewDialog::TrendViewDialog(QWidget *parent) : QDialog(parent)
{
    QString tmps = "QDialog {background-color: "+QString(MAINWINCLRA1)+";}";
    setStyleSheet(tmps);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::WindowMinMaxButtonsHint);
    TrendModel = 0;
    RangeChangeInProgress = false;
    Starting = true;
}

TrendViewDialog::~TrendViewDialog()
{

}

void TrendViewDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;

    if (!NoDiscrete)
    {
        vlyout->addWidget(WDFunc::NewLBL(this, "Дискретные сигналы"), 0, Qt::AlignCenter);
        SignalChooseWidget *scw = new SignalChooseWidget(DigitalNames);
        scw->setObjectName("digital");
        connect(scw,SIGNAL(SignalChoosed(QString)),this,SLOT(DSignalChoosed(QString)));
        connect(scw,SIGNAL(SignalToggled(QString,bool)),this,SLOT(DSignalToggled(QString,bool)));
        hlyout->addWidget(scw);
        vlyout->addWidget(PlotToolBar(), 0);
        vlyout->addWidget(DiscretePlot, 100);
        hlyout->addLayout(vlyout, 100);
        lyout->addLayout(hlyout);
    }
    if (!NoAnalog)
    {
        hlyout = new QHBoxLayout;
        vlyout = new QVBoxLayout;
        vlyout->addWidget(WDFunc::NewLBL(this, "Аналоговые сигналы"), 0, Qt::AlignCenter);
        SignalChooseWidget *scw = new SignalChooseWidget(AnalogNames);
        scw->setObjectName("analog");
        connect(scw,SIGNAL(SignalChoosed(QString)),this,SLOT(ASignalChoosed(QString)));
        connect(scw,SIGNAL(SignalToggled(QString,bool)),this,SLOT(ASignalToggled(QString,bool)));
        hlyout->addWidget(scw);
        vlyout->addWidget(PlotToolBar(), 0);
        vlyout->addWidget(AnalogPlot, 100);
        hlyout->addLayout(vlyout, 100);
        lyout->addLayout(hlyout);
    }
    hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(close()));
    hlyout->addStretch(10);
    hlyout->addWidget(pb);
    hlyout->addStretch(10);
    pb = new QPushButton("Сохранить в Excel");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SaveToExcel()));
    hlyout->addWidget(pb);
    hlyout->addStretch(10);
    pb = new QPushButton("Сохранить в Comtrade");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SaveToComtrade()));
    hlyout->addWidget(pb);
    hlyout->addStretch(10);
    lyout->addLayout(hlyout);
    setLayout(lyout);
}

QToolBar *TrendViewDialog::PlotToolBar()
{
    QToolBar *bar = new QToolBar;
    bar->setStyleSheet("QToolBar {background: 0px; margin: 0px; spacing: 5px; padding: 0px;}");
    bar->setIconSize(QSize(20,20));
    QAction *act = new QAction(this);
    act->setToolTip("Выбор осциллограмм");
    act->setIcon(QIcon("images/oscillogramm.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(Stage1_5()));
    bar->addAction(act);
    return bar;
}

QCPGraph *TrendViewDialog::GraphByName(QCustomPlot *plot, const QString &name)
{
    for (int i=0; i<plot->graphCount(); ++i)
    {
        QCPGraph *graph = plot->graph(i);
        if (graph->name() == name)
            return graph;
    }
    return 0;
}

void TrendViewDialog::graphClicked(QCPAbstractPlottable *plot, int dataIndex)
{
    double dataValue = plot->interface1D()->dataMainValue(dataIndex);
    QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.").arg(plot->name()).arg(dataIndex).arg(dataValue);
}

void TrendViewDialog::ASignalChoosed(QString signame)
{
    QCPGraph *graph = GraphByName(AnalogPlot, signame);
    if (graph == 0)
        return;
    graph->setSelection(QCPDataSelection(QCPDataRange()));
}

void TrendViewDialog::ASignalToggled(QString signame, bool isChecked)
{
    GraphSetVisible(AnalogPlot, signame, isChecked);
}

void TrendViewDialog::DSignalChoosed(QString signame)
{
    QCPGraph *graph = GraphByName(DiscretePlot, signame);
    if (graph == 0)
        return;
    graph->setSelection(QCPDataSelection(QCPDataRange()));
}

void TrendViewDialog::DSignalToggled(QString signame, bool isChecked)
{
    GraphSetVisible(DiscretePlot, signame, isChecked);
}

void TrendViewDialog::GraphSetVisible(QCustomPlot *plot, const QString &graphname, bool visible)
{
    if (!visible)
    {
        QCPGraph *graph = GraphByName(plot, graphname);
        if (graph == 0)
            return;
        plot->removeGraph(graph);
        plot->replot();
    }
    else
    {
        QCPGraph *graph = GraphByName(plot, graphname);
        if (graph == 0)
        {
            int count = plot->graphCount();
            QCPAxisRect *AxisRect = plot->axisRect();
            QCPGraph *graph = plot->addGraph(AxisRect->axis(QCPAxis::atBottom), AxisRect->axis(QCPAxis::atLeft));
            QPen pen;
            pen.setColor(QColor(qSin(count*1+1.2)*80+80, qSin(count*0.3+0)*80+80, qSin(count*0.3+1.5)*80+80));
            graph->setPen(pen);
            graph->keyAxis()->setLabel("Time, ms");
            graph->keyAxis()->setRange(XMin, XMax);
            graph->valueAxis()->setLabel(graphname);
            graph->setName(graphname);
            if (plot->objectName() == "digital")
            {
                graph->setData(TrendModel->MainPoints, TrendModel->DigitalMainData[graphname]);
                graph->setLineStyle(QCPGraph::lsStepLeft); // импульсы
            }
            else
            {
                graph->valueAxis()->setRange(YMin, YMax);
                graph->setData(TrendModel->MainPoints, TrendModel->AnalogMainData[graphname]);
            }
            graph->rescaleValueAxis(true, true);
            graph->rescaleKeyAxis();
        }
    }
    plot->replot();
}

void TrendViewDialog::DigitalRangeChanged(QCPRange range)
{
    if (RangeChangeInProgress || Starting)
    {
        RangeChangeInProgress = false;
        return;
    }
    RangeChangeInProgress = true;
    if (!NoAnalog)
    {
        AnalogPlot->xAxis->setRange(range);
        AnalogPlot->replot();
    }
}

void TrendViewDialog::AnalogRangeChanged(QCPRange range)
{
    if (RangeChangeInProgress || Starting)
    {
        RangeChangeInProgress = false;
        return;
    }
    RangeChangeInProgress = true;
    if (!NoDiscrete)
    {
        DiscretePlot->xAxis->setRange(range);
        DiscretePlot->replot();
    }
}

void TrendViewDialog::PlotShow()
{
    if (!NoAnalog)
    {
        for (int i=0; i<AnalogGraphs.keys().size(); ++i)
        {
            QString tmps = AnalogGraphs.keys().at(i);
            if (TrendModel->AContains(tmps))
            {
                AnalogGraphs[tmps]->setData(TrendModel->MainPoints, TrendModel->AnalogMainData[tmps]);
                AnalogGraphs[tmps]->rescaleAxes();
                AnalogGraphs[tmps]->rescaleValueAxis(true, true);
                AnalogGraphs[tmps]->rescaleKeyAxis();
                SignalChooseWidget *scw = this->findChild<SignalChooseWidget *>("analog");
                if ((scw != 0) && (i < AnalogNames.size()))
                    scw->SetChecked(AnalogNames.at(i), true);
            }
        }
        AnalogPlot->replot();
    }
    if (!NoDiscrete)
    {
        for (int i=0; i<DigitalGraphs.keys().size(); ++i)
        {
            QString tmps = DigitalGraphs.keys().at(i);
            if (TrendModel->DContains(tmps))
            {
                DigitalGraphs[tmps]->setData(TrendModel->MainPoints, TrendModel->DigitalMainData[tmps]);
                DigitalGraphs[tmps]->rescaleValueAxis(true, true);
                DigitalGraphs[tmps]->rescaleKeyAxis();
                SignalChooseWidget *scw = this->findChild<SignalChooseWidget *>("digital");
                if ((scw != 0) && (i < DigitalNames.size()))
                    scw->SetChecked(DigitalNames.at(i), true);
            }
        }
        DiscretePlot->replot();
    }
    this->showMaximized();
}

void TrendViewDialog::SetModel(TrendViewModel *model)
{
    TrendModel = model;
}

void TrendViewDialog::SetRanges(float XRangeMin, float XRangeMax, float YRangeMin, float YRangeMax)
{
    XMin = XRangeMin;
    XMax = XRangeMax;
    YMin = YRangeMin;
    YMax = YRangeMax;
}

void TrendViewDialog::SetDigitalNames(QStringList &names)
{
    DigitalNames = names;
}

void TrendViewDialog::SetAnalogNames(QStringList &names)
{
    AnalogNames = names;
}

void TrendViewDialog::SetupPlots()
{
    NoDiscrete = NoAnalog = false;
    QPen pen;
    int DigitalGraphNum = DigitalNames.size();
    int AnalogGraphNum = AnalogNames.size();
    if (DigitalGraphNum != 0)
    {
        DiscretePlot = new QCustomPlot;
        DiscretePlot->setObjectName("digital");
        DiscretePlot->setBackground(QBrush(QColor(ACONFYCLR)));
        QCPAxisRect *DigitalAxisRect = new QCPAxisRect(DiscretePlot);
        DiscretePlot->plotLayout()->clear();
        DiscretePlot->plotLayout()->addElement(0, 0, DigitalAxisRect);
        DiscretePlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
        connect(DiscretePlot->xAxis, SIGNAL(rangeChanged(QCPRange)),this,SLOT(DigitalRangeChanged(QCPRange)));
        DigitalAxisRect->setBackground(QBrush(QColor(ACONFYCLR)));
        int count = 0;
        while (count < DigitalGraphNum)
        {
            QCPGraph *graph = DiscretePlot->addGraph(DigitalAxisRect->axis(QCPAxis::atBottom), DigitalAxisRect->axis(QCPAxis::atLeft));
            pen.setColor(QColor(qSin(count*1+1.2)*80+80, qSin(count*0.3+0)*80+80, qSin(count*0.3+1.5)*80+80));
            graph->setPen(pen);
            graph->valueAxis()->setRange(-1, 2);
            graph->keyAxis()->setLabel("Time, ms");
            graph->keyAxis()->setRange(XMin, XMax);
            QString tmps = DigitalNames.at(count);
//            graph->valueAxis()->setLabel(tmps);
            graph->setName(tmps);
            graph->setLineStyle(QCPGraph::lsStepLeft); // импульсы
            DigitalGraphs[tmps] = graph;
            ++count;
        }
        DigitalAxisRect->insetLayout()->setMargins(QMargins(12, 12, 12, 12));
    }
    else
        NoDiscrete = true;
    if (AnalogGraphNum != 0)
    {
        AnalogPlot = new QCustomPlot;
        AnalogPlot->setObjectName("analog");
        AnalogPlot->setBackground(QBrush(QColor(ACONFYCLR)));
        QCPAxisRect *AnalogAxisRect = new QCPAxisRect(AnalogPlot);
        AnalogPlot->plotLayout()->clear();
        AnalogPlot->plotLayout()->addElement(0, 0, AnalogAxisRect);
        AnalogPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
        connect(AnalogPlot, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));
        connect(AnalogPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(AnalogRangeChanged(QCPRange)));
        AnalogAxisRect->setBackground(QBrush(QColor(ACONFYCLR)));
        int count = 0;
        while (count < AnalogGraphNum)
        {
            QCPGraph *graph = AnalogPlot->addGraph(AnalogAxisRect->axis(QCPAxis::atBottom), AnalogAxisRect->axis(QCPAxis::atLeft));
            pen.setColor(QColor(qSin(count*1+1.2)*80+80, qSin(count*0.3+0)*80+80, qSin(count*0.3+1.5)*80+80));
            graph->setPen(pen);
            graph->valueAxis()->setRange(YMin, YMax);
            graph->keyAxis()->setLabel("Time, ms");
            graph->keyAxis()->setRange(XMin, XMax);
            QString tmps = AnalogNames.at(count);
            graph->valueAxis()->setLabel(tmps);
            graph->setName(tmps);
            AnalogGraphs[tmps] = graph;
            ++count;
        }
        AnalogAxisRect->insetLayout()->setMargins(QMargins(12, 12, 12, 12));
    }
    else
        NoAnalog = true;
    Starting = false;
}
