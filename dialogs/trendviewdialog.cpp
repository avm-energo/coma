#include <QVector>
#include <QPen>
#include <QAction>
#include "QtXlsx/xlsxdocument.h"
#include "../config/config.h"
#include "trendviewdialog.h"

TrendViewDialog::TrendViewDialog(QWidget *parent) : QDialog(parent)
{
    QString tmps = "QDialog {background-color: "+QString(MAINWINCLRA1)+";}";
    setStyleSheet(tmps);
    setAttribute(Qt::WA_DeleteOnClose);
    TrendModel = 0;
}

TrendViewDialog::~TrendViewDialog()
{

}

void TrendViewDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;


    if (!NoDiscrete)
    {
        lyout->addWidget(PlotToolBar(), 0);
        lyout->addWidget(DiscretePlot, 100);
    }
    if (!NoAnalog)
    {
        lyout->addWidget(PlotToolBar(), 0);
        lyout->addWidget(AnalogPlot, 100);
    }
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

void TrendViewDialog::AnalogPlotContextMenu(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    if (AnalogPlot->legend->selectTest(pos, false) >= 0) // context menu on legend requested
    {
      menu->addAction("Move to top left", this, SLOT(MoveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignLeft));
      menu->addAction("Move to top center", this, SLOT(MoveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignHCenter));
      menu->addAction("Move to top right", this, SLOT(MoveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignRight));
      menu->addAction("Move to bottom right", this, SLOT(MoveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignRight));
      menu->addAction("Move to bottom left", this, SLOT(MoveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignLeft));
    }
/*    else  // general context menu on graphs requested
    {
      menu->addAction("Add random graph", this, SLOT(addRandomGraph()));
      if (Plot->selectedGraphs().size() > 0)
        menu->addAction("Remove selected graph", this, SLOT(removeSelectedGraph()));
      if (Plot->graphCount() > 0)
        menu->addAction("Remove all graphs", this, SLOT(removeAllGraphs()));
    } */

    menu->popup(AnalogPlot->mapToGlobal(pos));
}

void TrendViewDialog::DiscretePlotContextMenu(QPoint pos)
{

}

void TrendViewDialog::MoveLegend()
{
    if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
    {
      bool ok;
      int dataInt = contextAction->data().toInt(&ok);
      if (ok)
      {
        AnalogPlot->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
        AnalogPlot->replot();
      }
    }
}

void TrendViewDialog::ChooseGraphsToDisplay()
{

}

void TrendViewDialog::graphClicked(QCPAbstractPlottable *plot, int dataIndex)
{
    double dataValue = plot->interface1D()->dataMainValue(dataIndex);
    QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.").arg(plot->name()).arg(dataIndex).arg(dataValue);
}

void TrendViewDialog::PlotShow()
{
    int count = 0;
    while (count < AnalogGraphs.size())
    {
        if (count < TrendModel->ASize())
        {
            AnalogGraphs.at(count)->setData(TrendModel->MainPoints, TrendModel->AnalogMainData.at(count));
            AnalogGraphs.at(count)->rescaleKeyAxis();
            AnalogGraphs.at(count)->rescaleValueAxis();
        }
        ++count;
    }
    count = 0;
    while (count < DigitalGraphs.size())
    {
        if (count < TrendModel->DSize())
        {
            DigitalGraphs.at(count)->setData(TrendModel->MainPoints, TrendModel->DigitalMainData.at(count));
            DigitalGraphs.at(count)->rescaleKeyAxis();
            DigitalGraphs.at(count)->rescaleValueAxis();
        }
        ++count;
    }
    AnalogPlot->replot();
    DiscretePlot->replot();
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
    int row = 0;
    int DigitalGraphNum = DigitalNames.size();
    int AnalogGraphNum = AnalogNames.size();
    AnalogPlot = new QCustomPlot;
    AnalogPlot->setBackground(QBrush(QColor(ACONFYCLR)));
//    AnalogPlot->setContextMenuPolicy(Qt::CustomContextMenu);
//    connect(AnalogPlot,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(AnalogPlotContextMenu(QPoint)));
    AnalogPlot->plotLayout()->clear();
/*    QCPLegend *legend = new QCPLegend;
    AnalogPlot->legend = legend;
    AnalogPlot->legend->setLayer("legend");
    AnalogPlot->legend->setVisible(true);
    AnalogPlot->legend->setFont(QFont("Helvetica", 8));
    AnalogPlot->setAutoAddPlottableToLegend(true);
    AnalogPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectLegend); */
    AnalogPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    connect(AnalogPlot, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));

    if (DigitalGraphNum != 0)
    {
        DiscretePlot = new QCustomPlot;
        DiscretePlot->setBackground(QBrush(QColor(ACONFYCLR)));
//        DiscretePlot->setContextMenuPolicy(Qt::CustomContextMenu);
//        connect(DiscretePlot,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(DiscretePlotContextMenu(QPoint)));
        QCPAxisRect *DigitalAxisRect = new QCPAxisRect(DiscretePlot);
        DiscretePlot->plotLayout()->clear();
        DiscretePlot->plotLayout()->addElement(0, 0, DigitalAxisRect);
        QCPLegend *dlegend = new QCPLegend;
        DigitalAxisRect->insetLayout()->addElement(dlegend, Qt::AlignTop | Qt::AlignRight);
        DiscretePlot->legend = dlegend;
        DiscretePlot->legend->setLayer("legend");
        DiscretePlot->legend->setVisible(true);
        DiscretePlot->legend->setFont(QFont("Helvetica", 8));
        DiscretePlot->setAutoAddPlottableToLegend(true);
        DiscretePlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectLegend);
        DigitalAxisRect->setBackground(QBrush(QColor(ACONFYCLR)));
        DiscretePlot->plotLayout()->addElement(row++, 0, DigitalAxisRect);
        int count = 0;
        while (count < DigitalGraphNum)
        {
            QCPGraph *graph = DiscretePlot->addGraph(DigitalAxisRect->axis(QCPAxis::atBottom), DigitalAxisRect->axis(QCPAxis::atLeft));
            pen.setColor(QColor(qSin(count*1+1.2)*80+80, qSin(count*0.3+0)*80+80, qSin(count*0.3+1.5)*80+80));
            graph->setPen(pen);
            graph->valueAxis()->setRange(-1, 2);
            graph->keyAxis()->setLabel("Time, ns");
            graph->keyAxis()->setRange(XMin, XMax);
            graph->valueAxis()->setLabel(DigitalNames.at(count));
            graph->setName(DigitalNames.at(count));
            DigitalGraphs.append(graph);
            ++count;
        }
        DigitalAxisRect->insetLayout()->addElement(DiscretePlot->legend, Qt::AlignRight|Qt::AlignTop);
        DigitalAxisRect->insetLayout()->setMargins(QMargins(12, 12, 12, 12));
    }
    else
        NoDiscrete = true;
    if (AnalogGraphNum != 0)
    {
        QCPAxisRect *AnalogAxisRect = new QCPAxisRect(AnalogPlot);
        AnalogAxisRect->setBackground(QBrush(QColor(ACONFYCLR)));
        AnalogPlot->plotLayout()->addElement(row++, 0, AnalogAxisRect);
        int count = 0;
        while (count < AnalogGraphNum)
        {
            QCPGraph *graph = AnalogPlot->addGraph(AnalogAxisRect->axis(QCPAxis::atBottom), AnalogAxisRect->axis(QCPAxis::atLeft));
            pen.setColor(QColor(qSin(count*1+1.2)*80+80, qSin(count*0.3+0)*80+80, qSin(count*0.3+1.5)*80+80));
            graph->setPen(pen);
            graph->valueAxis()->setRange(YMin, YMax);
            graph->keyAxis()->setLabel("Time, ns");
            graph->keyAxis()->setRange(XMin, XMax);
            graph->valueAxis()->setLabel(AnalogNames.at(count));
            graph->setName(AnalogNames.at(count));
            AnalogGraphs.append(graph);
            ++count;
        }
        AnalogAxisRect->insetLayout()->addElement(AnalogPlot->legend, Qt::AlignRight|Qt::AlignTop);
        AnalogAxisRect->insetLayout()->setMargins(QMargins(12, 12, 12, 12));
    }
    else
        NoAnalog = true;
}
