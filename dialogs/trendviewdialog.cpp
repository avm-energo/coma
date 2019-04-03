/* TrendViewDialog - dialog with trendview to show the oscillogram
 * Using:
 *          TrendViewModel *TModel = new TrendViewModel(tmpdv, tmpav, OHD.len);
 *          dlg->SetModel(TModel);
 *          dlg->SetAnalogNames(tmpav);
 *          dlg->SetDigitalNames(tmpdv);
 *          dlg->SetAnalogColors(acolors);
 *          dlg->SetDigitalColors(dcolors);
 *          dlg->SetRanges(xmin, xmax, -200, 200);
 *          dlg->SetupPlots();
 *          dlg->SetupUI();
 *          if (!TModel->SetPointsAxis(xmin, OHD.step))
 *              return GENERALERROR;
 *          for (quint32 i = 0; i < OHD.len; ++i) // цикл по точкам
 *          {
 *              Point8x point;
 *              if (!PosPlusPlus(&point, sizeof(Point8x)))
 *                  return GENERALERROR;
 *              for (int i=0; i<6; ++i)
 *                  TModel->AddAnalogPoint(tmpav.at(i), point.An[i]);
 *          }
 *          TModel->SetFilename(fn);
 *          dlg->setModal(false);
 *          dlg->PlotShow();
 *          dlg->show();
 */

#include <QVector>
#include <QPen>
#include <QAction>
#include <algorithm>
#include "../config/config.h"
#include "../widgets/signalchoosewidget.h"
#include "../widgets/wd_func.h"
#include "../gen/error.h"
#include "../gen/colors.h"
#include "../gen/files.h"
#include "trendviewdialog.h"
#include "../gen/stdfunc.h"
#include "../check/checkdialog85.h"

TrendViewDialog::TrendViewDialog(QByteArray &ba, QWidget *parent) : QDialog(parent)
{
    QString tmps = "QDialog {background-color: "+QString(MAINWINCLRA1)+";}";
    setStyleSheet(tmps);
    //setAttribute(Qt::WA_DeleteOnClose);
    //setWindowFlags(Qt::WindowMinMaxButtonsHint);
    TrendModel = nullptr;
    RangeChangeInProgress = false;
    RangeAxisInProgress = false;
    Starting = true;
    BAToSave = ba;
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
        SignalChooseWidget *scw = new SignalChooseWidget(DigitalDescription.Names, DiscriptionsDicrete);
        scw->setObjectName("digital");
        //scw->setAccessibleDescription("Descr");
        connect(scw,SIGNAL(SignalChoosed(QString)),this,SLOT(DSignalChoosed(QString)));
        connect(scw,SIGNAL(SignalToggled(QString,bool)),this,SLOT(DSignalToggled(QString,bool)));
        vlyout->addWidget(scw);
    }
    if (!NoAnalog)
    {
        SignalChooseWidget *scw = new SignalChooseWidget(AnalogDescription.Names, DiscriptionsAnalog);
        scw->setObjectName("analog");
        connect(scw,SIGNAL(SignalChoosed(QString)),this,SLOT(ASignalChoosed(QString)));
        connect(scw,SIGNAL(SignalToggled(QString,bool)),this,SLOT(ASignalToggled(QString,bool)));
        vlyout->addWidget(scw);
    }
    hlyout->addLayout(vlyout);
    if (MainPlot.isNull())
    {
        DBGMSG;
        return;
    }
    hlyout->addWidget(MainPlot, 100);
    lyout->addLayout(hlyout);
    hlyout = new QHBoxLayout;
    /*QPushButton *pb = new QPushButton("Сохранить в Excel");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SaveToExcel()));
    hlyout->addWidget(pb);
    hlyout->addStretch(10);
    pb = new QPushButton("Сохранить в Comtrade");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SaveToComtrade()));
    hlyout->addWidget(pb);
    hlyout->addStretch(10);
    pb = new QPushButton("Сохранить в файл .osc");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(SaveToOsc()));
    hlyout->addWidget(pb);
    hlyout->addStretch(10);
    pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(close()));
    hlyout->addWidget(pb);*/
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
    act->setIcon(QIcon("images/osc.png"));
    connect(act,SIGNAL(triggered()),this,SLOT(Stage1_5()));
    bar->addAction(act);
    return bar;
}

QCPGraph *TrendViewDialog::GraphByName(const QString &name)
{
    for (int i=0; i<MainPlot->graphCount(); ++i)
    {
        QCPGraph *graph = MainPlot->graph(i);
        if (graph->name() == name)
            return graph;
    }
    return nullptr;
}

void TrendViewDialog::ChangeRange(QCPRange range)
{
    if (RangeChangeInProgress || Starting)
    {
        RangeChangeInProgress = false;
        return;
    }
    RangeChangeInProgress = true;
    QList<QCPAxisRect *> axisrects = MainPlot->axisRects();
    foreach(QCPAxisRect *rect, axisrects)
        rect->axis(QCPAxis::atBottom)->setRange(range);
    MainPlot->replot();
}

void TrendViewDialog::ChangeAxisRange(QCPRange range)
{
    if (RangeAxisInProgress || Starting)
    {
        RangeAxisInProgress = false;
        return;
    }
    RangeAxisInProgress = true;
    QList<QCPAxisRect *> axisrects = MainPlot->axisRects();
    foreach(QCPAxisRect *rect, axisrects)
    rect->axis(QCPAxis::atLeft, 1)->setRange(range);
    MainPlot->replot();
}

QCPLegend *TrendViewDialog::SetLegend(int rectindex)
{
    // setup an extra legend for that axis rect:
    QCPLegend *rectlegend = new QCPLegend;
    // now we move the legend from the inset layout of the axis rect into the main grid layout.
    // We create a sub layout so we can generate a small gap between the plot layout cell border
    // and the legend border:
    QCPLayoutGrid *subLayout = new QCPLayoutGrid;
    MainPlot->plotLayout()->addElement(rectindex, 0, subLayout);
    subLayout->setMargins(QMargins(5, 0, 5, 5));
    subLayout->addElement(0, 0, rectlegend);
    // change the fill order of the legend, so it's filled left to right in columns:
    rectlegend->setFillOrder(QCPLegend::foColumnsFirst);
    // set legend's row stretch factor very small so it ends up with minimum height:
    MainPlot->plotLayout()->setRowStretchFactor(rectindex, 0.001);
    rectlegend->setLayer("legend");
    return rectlegend;
}

void TrendViewDialog::graphClicked(QCPAbstractPlottable *plot, int dataIndex)
{
    double dataValue = plot->interface1D()->dataMainValue(dataIndex);
    QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.").arg(plot->name()).arg(dataIndex).arg(dataValue);
}

void TrendViewDialog::ASignalChoosed(QString signame)
{
    QCPGraph *graph = GraphByName(signame);
    if (graph == nullptr)
        return;
    graph->setSelection(QCPDataSelection(QCPDataRange()));
}

void TrendViewDialog::ASignalToggled(QString signame, bool isChecked)
{
    GraphSetVisible(AnalogRectIndex, signame, isChecked);
}

void TrendViewDialog::DSignalChoosed(QString signame)
{
    QCPGraph *graph = GraphByName(signame);
    if (graph == nullptr)
        return;
    graph->setSelection(QCPDataSelection(QCPDataRange()));
}

void TrendViewDialog::DSignalToggled(QString signame, bool isChecked)
{
    GraphSetVisible(DiscreteRectIndex, signame, isChecked);
}

void TrendViewDialog::GraphSetVisible(int rectindex, const QString &graphname, bool visible)
{
    if (!visible)
    {
        QCPGraph *graph = GraphByName(graphname);
        if (graph == nullptr)
            return;
        MainPlot->removeGraph(graph);
        if (rectindex == AnalogRectIndex)
        {
            AnalogGraphs.remove(graphname);
            AnalogLegend->remove(AnalogLegend->itemWithPlottable(graph));
        }
        else if (rectindex == DiscreteRectIndex)
        {
            DigitalGraphs.remove(graphname);
            DiscreteLegend->remove(DiscreteLegend->itemWithPlottable(graph));
        }
        else
            DBGMSG;
        MainPlot->replot();
    }
    else
    {
        SignalChooseWidget *scw = this->findChild<SignalChooseWidget *>("digital");
        if (((rectindex == DiscreteRectIndex) && (DigitalGraphs.size() < MAXGRAPHSPERPLOT)) || \
                ((rectindex == AnalogRectIndex) && (AnalogGraphs.size() < MAXGRAPHSPERPLOT)))
        {
            if (scw != nullptr)
                scw->SetChecked(graphname, true);
            QCPGraph *graph = GraphByName(graphname);
            if (graph == nullptr)
            {
                int count = MainPlot->graphCount();
                QCPAxisRect *AxisRect = MainPlot->axisRect(rectindex);
                QCPGraph *graph = MainPlot->addGraph(AxisRect->axis(QCPAxis::atBottom), AxisRect->axis(QCPAxis::atLeft));
                QPen pen;
/*                graph->keyAxis()->setLabel("Time, ms");
                graph->keyAxis()->setRange(XMin, XMax);
                graph->valueAxis()->setLabel(graphname); */
                graph->setName(graphname);
                if (rectindex == DiscreteRectIndex)
                {
                    graph->setData(TrendModel->MainPoints, TrendModel->DigitalMainData[graphname]);
                    graph->setLineStyle(QCPGraph::lsStepLeft); // импульсы
                    DiscreteLegend->addItem(new QCPPlottableLegendItem(DiscreteLegend, graph));
                    DigitalGraphs[graphname] = graph;
                    if (!DigitalDescription.Colors[graphname].isEmpty())
                        pen.setColor(QColor(DigitalDescription.Colors[graphname]));
                    else
                        pen.setColor(QColor(qSin(count*1+1.2)*80+80, qSin(count*0.3+0)*80+80, qSin(count*0.3+1.5)*80+80));
                }
                else
                {
/*                    double min = *std::min_element(TrendModel->AnalogMainData[graphname].constBegin(), \
                                                   TrendModel->AnalogMainData[graphname].constEnd());
                    double max = *std::max_element(TrendModel->AnalogMainData[graphname].constBegin(), \
                                                   TrendModel->AnalogMainData[graphname].constEnd());
                    graph->valueAxis()->setRange(min, max); */
                    graph->setData(TrendModel->MainPoints, TrendModel->AnalogMainData[graphname]);
                    AnalogLegend->addItem(new QCPPlottableLegendItem(AnalogLegend, graph));
                    AnalogGraphs[graphname] = graph;
                    if (!AnalogDescription.Colors[graphname].isEmpty())
                        pen.setColor(QColor(AnalogDescription.Colors[graphname]));
                    else
                        pen.setColor(QColor(qSin(count*1+1.2)*80+80, qSin(count*0.3+0)*80+80, qSin(count*0.3+1.5)*80+80));
                }
                graph->setPen(pen);
//                graph->rescaleValueAxis(true, true);
//                graph->rescaleKeyAxis();
            }
        }
        else
            scw->SetChecked(graphname, false);
    }
    MainPlot->replot();
}

void TrendViewDialog::DigitalRangeChanged(QCPRange range)
{
    if (!NoAnalog)
        ChangeRange(range);
}

void TrendViewDialog::AxisRangeChanged(QCPRange range)
{
        if(!NoDiscrete)
        ChangeAxisRange(range);
}


void TrendViewDialog::AnalogRangeChanged(QCPRange range)
{
    if (!NoDiscrete)
        ChangeRange(range);
}

void TrendViewDialog::SaveToExcel()
{
    TrendModel->SaveToExcel(this);
}

void TrendViewDialog::SaveToComtrade()
{

}

void TrendViewDialog::SaveToOsc()
{
    QString filename = Files::ChooseFileForSave(this, "Файлы осциллограмм (*.osc)", "osc");
    Files::SaveToFile(filename, BAToSave, BAToSave.size());
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
                if ((scw != nullptr) && (i < AnalogDescription.Names.size()))
                    scw->SetChecked(AnalogDescription.Names.at(i), true);
            }
        }
/*        QCPItemTracer *itr = new QCPItemTracer(AnalogPlot);
        itr->position->setCoords(0, 0); */
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
                if ((scw != nullptr) && (i < DigitalDescription.Names.size()))
                    scw->SetChecked(DigitalDescription.Names.at(i), true);
            }
        }
/*        QCPItemTracer *itr = new QCPItemTracer(DiscretePlot);
        itr->position->setCoords(0, 0); */
    }
    QCPMarginGroup *group = new QCPMarginGroup(MainPlot);
    MainPlot->axisRect(0)->setMarginGroup(QCP::msLeft, group);
    switch (TrendModel->idOsc)
    {
        case MT_ID85:
            MainPlot->axisRect(1)->setMarginGroup(QCP::msLeft, group);
        break;
    }
    MainPlot->replot();
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
    DigitalDescription.Names = names;
}

void TrendViewDialog::SetAnalogNames(QStringList &names)
{
    AnalogDescription.Names = names;
}

void TrendViewDialog::SetAnalogDescriptions(QStringList &descr)
{
    DiscriptionsAnalog= descr;
}

void TrendViewDialog::SetDiscreteDescriptions(QStringList &descr)
{
    DiscriptionsDicrete = descr;
}

void TrendViewDialog::SetDigitalColors(QStringList &colors)
{
    for (int i=0; i<DigitalDescription.Names.size(); ++i)
    {
        QString tmps = DigitalDescription.Names.at(i);
        if (i < colors.size())
            DigitalDescription.Colors[tmps] = colors.at(i);
        else
            DigitalDescription.Colors[tmps] = "";
    }
}

void TrendViewDialog::SetAnalogColors(QStringList &colors)
{
    for (int i=0; i<AnalogDescription.Names.size(); ++i)
    {
        QString tmps = AnalogDescription.Names.at(i);
        if (i < colors.size())
            AnalogDescription.Colors[tmps] = colors.at(i);
        else
            AnalogDescription.Colors[tmps] = "";
    }
}

void TrendViewDialog::SetupPlots()
{
    NoDiscrete = NoAnalog = false;
    QPen pen;
    int DigitalGraphNum = DigitalDescription.Names.size();
    int AnalogGraphNum = AnalogDescription.Names.size();
    MainPlot = new QCustomPlot;
    MainPlot->setBackground(QBrush(QColor(ACONFYCLR)));
    MainPlot->plotLayout()->clear();
    MainPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    MainPlot->setAutoAddPlottableToLegend(false);
    int MainPlotLayoutRow = 0;
    int RectIndex = 0;
    if (DigitalGraphNum != 0)
    {
        QCPTextElement *title = new QCPTextElement(MainPlot);
        title->setText("Дискретные сигналы");
        title->setFont(QFont("sans", 12, QFont::Bold));
        MainPlot->plotLayout()->addElement(MainPlotLayoutRow++, 0, title); // place the title in the empty cell we've just created

        QCPAxisRect *DigitalAxisRect = new QCPAxisRect(MainPlot);
        DiscreteRectIndex = RectIndex++;
        MainPlot->plotLayout()->addElement(MainPlotLayoutRow++, 0, DigitalAxisRect);
        DiscreteLegend = SetLegend(MainPlotLayoutRow++);
        DigitalAxisRect->setBackground(QBrush(QColor(ACONFYCLR)));
        int count = 0;
        while ((count < DigitalGraphNum) && (DigitalGraphs.size() < MAXGRAPHSPERPLOT))
        {
            QString tmps = DigitalDescription.Names.at(count);
            QCPGraph *graph = MainPlot->addGraph(DigitalAxisRect->axis(QCPAxis::atBottom), DigitalAxisRect->axis(QCPAxis::atLeft, 0));
            if (!DigitalDescription.Colors[tmps].isEmpty())
                pen.setColor(QColor(DigitalDescription.Colors[tmps]));
            else
                pen.setColor(QColor(qSin(count*1+1.2)*80+80, qSin(count*0.3+0)*80+80, qSin(count*0.3+1.5)*80+80));
            graph->setPen(pen);
            graph->valueAxis()->setRange(-1, 2);
            graph->keyAxis()->setLabel("Time, ms");
            graph->keyAxis()->setRange(XMin, XMax);
//            graph->valueAxis()->setLabel(tmps);
            graph->setName(tmps);
            graph->setLineStyle(QCPGraph::lsStepLeft); // импульсы
            DigitalGraphs[tmps] = graph;
            DiscreteLegend->addItem(new QCPPlottableLegendItem(DiscreteLegend, graph));




            ++count;
        }
        DigitalAxisRect->insetLayout()->setMargins(QMargins(12, 12, 12, 12));
        connect(DigitalAxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)),this,SLOT(DigitalRangeChanged(QCPRange)));
    }
    else
        NoDiscrete = true;
    if (AnalogGraphNum != 0)
    {
        QCPTextElement *title = new QCPTextElement(MainPlot);
        title->setText("Аналоговые сигналы");
        title->setFont(QFont("sans", 12, QFont::Bold));
        MainPlot->plotLayout()->addElement(MainPlotLayoutRow++, 0, title); // place the title in the empty cell we've just created
        QCPAxisRect *AnalogAxisRect = new QCPAxisRect(MainPlot);
        //AnalogAxisRect->addAxis(QCPAxis::atLeft);
        AnalogRectIndex = RectIndex++;
        MainPlot->plotLayout()->addElement(MainPlotLayoutRow++, 0, AnalogAxisRect);
        AnalogLegend = SetLegend(MainPlotLayoutRow++);
        AnalogAxisRect->setBackground(QBrush(QColor(ACONFYCLR)));
        int count = 0;
        switch(TrendModel->idOsc)
        {
          case MT_ID85:

            while ((count < AnalogGraphNum) && (AnalogGraphs.size() < MAXGRAPHSPERPLOT))
            {
/*                QString tmps = AnalogDescription.Names.at(count); */
                QString tmps = AnalogDescription.Names.at(count);
                QCPGraph *graph;
                if(count<3 || count>5)
                    graph = MainPlot->addGraph(AnalogAxisRect->axis(QCPAxis::atBottom), AnalogAxisRect->axis(QCPAxis::atLeft));
                else
                    graph = MainPlot->addGraph(AnalogAxisRect->axis(QCPAxis::atBottom),  AnalogAxisRect->axis(QCPAxis::atLeft, 1));
                if (!AnalogDescription.Colors[tmps].isEmpty())
                    pen.setColor(QColor(AnalogDescription.Colors[tmps]));
                else
                    pen.setColor(QColor(qSin(count*1+1.2)*80+80, qSin(count*0.3+0)*80+80, qSin(count*0.3+1.5)*80+80));
                graph->setPen(pen);
                graph->valueAxis()->setRange(YMin, YMax);
                graph->keyAxis()->setLabel("Time, ms");
                graph->keyAxis()->setRange(XMin, XMax);
                graph->valueAxis()->setLabel(tmps);
                graph->setName(tmps);
                AnalogGraphs[tmps] = graph;
                AnalogLegend->addItem(new QCPPlottableLegendItem(AnalogLegend, graph));


                ++count;
            }

             //connect(AnalogAxisRect->axis(QCPAxis::atLeft, 0), SIGNAL(rangeChanged(QCPRange)),this,SLOT(AxisRangeChanged(QCPRange)));

            break;

          default:

            while ((count < AnalogGraphNum) && (AnalogGraphs.size() < MAXGRAPHSPERPLOT))
            {
                QString tmps = AnalogDescription.Names.at(count);
                QCPGraph *graph = MainPlot->addGraph(AnalogAxisRect->axis(QCPAxis::atBottom), AnalogAxisRect->axis(QCPAxis::atLeft));
                if (!AnalogDescription.Colors[tmps].isEmpty())
                    pen.setColor(QColor(AnalogDescription.Colors[tmps]));
                else
                    pen.setColor(QColor(qSin(count*1+1.2)*80+80, qSin(count*0.3+0)*80+80, qSin(count*0.3+1.5)*80+80));
                graph->setPen(pen);
                graph->valueAxis()->setRange(YMin, YMax);
                if ((TrendModel->idOsc > ID_OSC_CH0) && (TrendModel->idOsc < ID_OSC_CH7))
                {
                    graph->keyAxis()->setLabel("Time, mcs");
                }
                else
                {
                    graph->keyAxis()->setLabel("Time, ms");
                }
                graph->keyAxis()->setRange(XMin, XMax);
    //            graph->valueAxis()->setLabel(tmps);
                graph->setName(tmps);
                AnalogGraphs[tmps] = graph;
                AnalogLegend->addItem(new QCPPlottableLegendItem(AnalogLegend, graph));

                ++count;
            }
            break;



        }

        AnalogAxisRect->insetLayout()->setMargins(QMargins(12, 12, 12, 12));
        connect(AnalogAxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)),this,SLOT(AnalogRangeChanged(QCPRange)));

    }
    else
        NoAnalog = true;
    connect(MainPlot, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));
    Starting = false;
}
