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
    AnalogRescaleActivated = false;
    DigitalRescaleActivated = false;
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
        vlyout->addWidget(PlotToolBar(ST_DIGITAL));
        SignalChooseWidget *scw = new SignalChooseWidget(DigitalDescription.Names, DigitalDescription.Descriptions);
        scw->setObjectName("digital");
        //scw->setAccessibleDescription("Descr");
        connect(scw,SIGNAL(SignalChoosed(QString)),this,SLOT(SignalChoosed(QString)));
        connect(scw,SIGNAL(SignalToggled(QString,bool)),this,SLOT(SignalToggled(QString,bool)));
        vlyout->addWidget(scw);
    }
    if (!NoAnalog)
    {
        vlyout->addWidget(PlotToolBar(ST_ANALOG));
        SignalChooseWidget *scw = new SignalChooseWidget(AnalogDescription.Names, AnalogDescription.Descriptions);
        scw->setObjectName("analog");
        connect(scw,SIGNAL(SignalChoosed(QString)),this,SLOT(SignalChoosed(QString)));
        connect(scw,SIGNAL(SignalToggled(QString,bool)),this,SLOT(SignalToggled(QString,bool)));
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
/*    hlyout = new QHBoxLayout;
    QPushButton *pb = new QPushButton("Сохранить в Excel");
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
    hlyout->addWidget(pb);
    lyout->addLayout(hlyout); */
    setLayout(lyout);
}

QToolBar *TrendViewDialog::PlotToolBar(SignalTypes type)
{
    QToolBar *bar = new QToolBar;
    bar->setStyleSheet("QToolBar {background: 0px; margin: 0px; spacing: 5px; padding: 0px;}");
    bar->setIconSize(QSize(20,20));
    QAction *act = new QAction(this);
    act->setCheckable(true);
    act->setData(type);
    act->setToolTip("Выбор осциллограмм");
    act->setIcon(QIcon("images/rescale.png"));
    connect(act,SIGNAL(triggered(bool)),this,SLOT(SetRescale(bool)));
    bar->addAction(act);
    return bar;
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

int TrendViewDialog::SignalOscDescriptionSize(TrendViewDialog::SignalTypes type)
{
    int count = 0;
    foreach (SignalOscPropertiesStruct st, SignalOscPropertiesMap)
    {
        if (st.Type == type)
            ++count;
    }
    return count;
}

void TrendViewDialog::graphClicked(QCPAbstractPlottable *plot, int dataIndex)
{
    double dataValue = plot->interface1D()->dataMainValue(dataIndex);
    QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.").arg(plot->name()).arg(dataIndex).arg(dataValue);
}

void TrendViewDialog::SignalChoosed(QString signame)
{
    if (SignalOscPropertiesMap.keys().contains(signame))
    {
        QCPGraph *graph = (SignalOscPropertiesMap[signame].Graph);
        if (graph == nullptr)
            return;
        graph->setSelection(QCPDataSelection(QCPDataRange()));
    }
}

void TrendViewDialog::SignalToggled(QString signame, bool isChecked)
{
    if (SignalOscPropertiesMap.keys().contains(signame))
    {
        if ((!isChecked) && SignalOscPropertiesMap[signame].Visible) // signal to be deleted from plot
        {
            QCPGraph *graph = SignalOscPropertiesMap[signame].Graph;
            if (graph == nullptr)
                return;
            MainPlot->removeGraph(graph);
            SignalOscPropertiesMap[signame].Graph = nullptr;
            SignalTypes st = SignalOscPropertiesMap[signame].Type;
            if (st == ST_ANALOG)
                AnalogLegend->remove(AnalogLegend->itemWithPlottable(graph));
            else if (st == ST_DIGITAL)
                DiscreteLegend->remove(DiscreteLegend->itemWithPlottable(graph));
            else
                DBGMSG;
            SignalOscPropertiesMap[signame].Visible = false;
        }
        else // signal to be added to plot
        {
            if (SignalOscPropertiesMap[signame].Visible) // visible already
                return;
            SignalChooseWidget *scw = this->findChild<SignalChooseWidget *>("digital");
            int analogcount = SignalOscDescriptionSize(ST_ANALOG);
            int digitalcount = SignalOscDescriptionSize(ST_DIGITAL);
            if (((SignalOscPropertiesMap[signame].Type == ST_DIGITAL) && (digitalcount < MAXGRAPHSPERPLOT)) || \
                    ((SignalOscPropertiesMap[signame].Type == ST_ANALOG) && (analogcount < MAXGRAPHSPERPLOT)))
            {
                if (scw != nullptr)
                    scw->SetChecked(signame, true);
                QCPGraph *graph = SignalOscPropertiesMap[signame].Graph;
                if (graph == nullptr)
                {
                    int count = MainPlot->graphCount();
                    SignalTypes type = SignalOscPropertiesMap[signame].Type;
                    QCPAxisRect *AxisRect = (type == ST_ANALOG) ? \
                                MainPlot->axisRect(1) : MainPlot->axisRect(0);
                    int axisindex = (signame.at(0) == "I") ? 1 : 0;
                    QCPGraph *graph = MainPlot->addGraph(AxisRect->axis(QCPAxis::atBottom), AxisRect->axis(QCPAxis::atLeft, axisindex));
                    QPen pen;
    /*                graph->keyAxis()->setLabel("Time, ms");
                    graph->keyAxis()->setRange(XMin, XMax);
                    graph->valueAxis()->setLabel(graphname); */
                    graph->setName(signame);
                    if (type == ST_DIGITAL)
                    {
                        graph->setData(TrendModel->MainPoints, TrendModel->DigitalMainData[signame]);
                        graph->setLineStyle(QCPGraph::lsStepLeft); // импульсы
                        DiscreteLegend->addItem(new QCPPlottableLegendItem(DiscreteLegend, graph));
                        if (!DigitalDescription.Colors[signame].isEmpty())
                            pen.setColor(QColor(DigitalDescription.Colors[signame]));
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
                        graph->setData(TrendModel->MainPoints, TrendModel->AnalogMainData[signame]);
                        AnalogLegend->addItem(new QCPPlottableLegendItem(AnalogLegend, graph));
                        if (!AnalogDescription.Colors[signame].isEmpty())
                            pen.setColor(QColor(AnalogDescription.Colors[signame]));
                        else
                            pen.setColor(QColor(qSin(count*1+1.2)*80+80, qSin(count*0.3+0)*80+80, qSin(count*0.3+1.5)*80+80));
                    }
                    graph->setPen(pen);
    //                graph->rescaleValueAxis(true, true);
    //                graph->rescaleKeyAxis();
                }
            }
            else
                scw->SetChecked(signame, false);
        }
        MainPlot->replot();
    }
}

void TrendViewDialog::DigitalRangeChanged(QCPRange range)
{
    if (!NoAnalog)
        ChangeRange(range);
}

void TrendViewDialog::MouseWheel()
{
    // https://stackoverflow.com/questions/36807980/qcustomplot-mouse-interaction-on-secondary-axis
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed
    QList<QCPAxisRect *> axisrects = MainPlot->axisRects();
    foreach(QCPAxisRect *rect, axisrects)
    {
        QCPAxis *axis = rect->axis(QCPAxis::atBottom, 0);
        if ((axis != nullptr) && (axis->selectedParts().testFlag(QCPAxis::spAxis)))
        {
            rect->setRangeZoomAxes(axis, rect->axis(QCPAxis::atLeft, 0));
            rect->setRangeZoom(axis->orientation());
            continue;
        }
        axis = rect->axis(QCPAxis::atLeft, 0);
        if ((axis != nullptr) && (axis->selectedParts().testFlag(QCPAxis::spAxis)))
        {
            rect->setRangeZoomAxes(rect->axis(QCPAxis::atBottom, 0), axis);
            rect->setRangeZoom(axis->orientation());
            continue;
        }
        axis = rect->axis(QCPAxis::atLeft, 1);
        if ((axis != nullptr) && (axis->selectedParts().testFlag(QCPAxis::spAxis)))
        {
            rect->setRangeZoomAxes(rect->axis(QCPAxis::atBottom, 0), axis);
            rect->setRangeZoom(axis->orientation());
            continue;
        }
        else
          rect->setRangeZoom(0); // deny zoom when none of axes selected
        // check for automatic rescaling enabled
    }
    if (DigitalRescaleActivated)
    {
        // don't know how to rescale digital axes
    }
    if (AnalogRescaleActivated)
    {
        // suppose that the analog rect has index 1
        QCPAxisRect *rect = MainPlot->axisRect(1);
        if (rect != nullptr)
        {
            AutoResizeRange(rect, 0);
//            AutoResizeRange(rect, 1);
        }
    }
    MainPlot->replot();
}

void TrendViewDialog::MousePress()
{
/*    QList<QCPAxis*> axesList;

    // https://github.com/mmccullo/qcustomplot-interaction-example/blob/master/mainwindow.cpp
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, check if a graph is selected and drag both axes

    QList<QCPAxisRect *> axisrects = MainPlot->axisRects();
    foreach(QCPAxisRect *rect, axisrects)
    {
        axesList.clear();
        foreach(QCPAxis *axis, rect->axes())
        {
            if ((axis != nullptr) && (axis->selectedParts().testFlag(QCPAxis::spAxis)))
                axesList.append(axis);
        }

        bool selectedGraph = false;

        // is a graph selected?
        QList<QCPGraph *> GraphList = rect->graphs();
        for (int i = 0; i < GraphList.size(); ++i)
        {
            QCPGraph *graph = GraphList.at(i);
            if (graph->selected())
            {
                selectedGraph = true;
                axesList.append(graph->keyAxis());
                axesList.append(graph->valueAxis());
                break;
            }
        }

        // set the axes to drag
        rect->setRangeDragAxes(axesList);
    } */
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

void TrendViewDialog::SetRescale(bool isChecked)
{
    QAction *act = qobject_cast<QAction *>(sender());
    if (act != nullptr)
    {
        int tmps = act->data().toInt();
        if (tmps == ST_ANALOG)
            AnalogRescaleActivated = isChecked;
        else
            DigitalRescaleActivated = isChecked;
    }
}

void TrendViewDialog::AutoResizeRange(QCPAxisRect *rect, int index)
{
    double min = qAbs(rect->axis(QCPAxis::atLeft, index)->range().lower);
    double max = qAbs(rect->axis(QCPAxis::atLeft, index)->range().upper);
    if (min < max)
        rect->axis(QCPAxis::atLeft, index)->setRange(-min, min);
    else
        rect->axis(QCPAxis::atLeft, index)->setRange(-max, max);
}

void TrendViewDialog::PlotShow()
{
    if (!NoAnalog)
    {
        for (int i=0; i<SignalOscPropertiesMap.keys().size(); ++i)
        {
            QString tmps = SignalOscPropertiesMap.keys().at(i);
            if (SignalOscPropertiesMap[tmps].Visible)
            {
                QCPGraph *graph = SignalOscPropertiesMap[tmps].Graph;
                if (graph == nullptr)
                    continue;
                if (TrendModel->AContains(tmps))
                {
                    graph->setData(TrendModel->MainPoints, TrendModel->AnalogMainData[tmps]);
                    graph->rescaleAxes();
                    graph->rescaleValueAxis(true, true);
                    graph->rescaleKeyAxis();
                    SignalChooseWidget *scw = this->findChild<SignalChooseWidget *>("analog");
                    if (scw != nullptr)
                        scw->SetChecked(tmps, true);
                }
                if (TrendModel->DContains(tmps))
                {
                    graph->setData(TrendModel->MainPoints, TrendModel->DigitalMainData[tmps]);
                    graph->rescaleValueAxis(true, true);
                    graph->rescaleKeyAxis();
                    SignalChooseWidget *scw = this->findChild<SignalChooseWidget *>("digital");
                    if (scw != nullptr)
                        scw->SetChecked(tmps, true);
                }
            }
        }
/*        QCPItemTracer *itr = new QCPItemTracer(AnalogPlot);
        itr->position->setCoords(0, 0); */
    }
    QCPMarginGroup *group = new QCPMarginGroup(MainPlot);
    QCPAxisRect *rect = MainPlot->axisRect(0);
    if (rect != nullptr)
        rect->setMarginGroup(QCP::msLeft, group);
    switch (TrendModel->idOsc)
    {
    case MT_ID85:
    {
        rect = MainPlot->axisRect(1);
        if (rect != nullptr)
            rect->setMarginGroup(QCP::msLeft, group);
        break;
    }
    default:
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
    AnalogDescription.Descriptions = descr;
}

void TrendViewDialog::SetDiscreteDescriptions(QStringList &descr)
{
    DigitalDescription.Descriptions = descr;
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
    MainPlot->setInteractions(QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectAxes);
    MainPlot->setAutoAddPlottableToLegend(false);
    int MainPlotLayoutRow = 0;
//    int RectIndex = 0;
    if (DigitalGraphNum != 0)
    {
        QCPTextElement *title = new QCPTextElement(MainPlot);
        title->setText("Дискретные сигналы");
        title->setFont(QFont("sans", 12, QFont::Bold));
        MainPlot->plotLayout()->addElement(MainPlotLayoutRow++, 0, title); // place the title in the empty cell we've just created
        QCPAxisRect *DigitalAxisRect = new QCPAxisRect(MainPlot);
//        DiscreteRectIndex = RectIndex++;
        MainPlot->plotLayout()->addElement(MainPlotLayoutRow++, 0, DigitalAxisRect);
        DiscreteLegend = SetLegend(MainPlotLayoutRow++);
        DigitalAxisRect->setBackground(QBrush(QColor(ACONFYCLR)));
        int count = 0;
        SignalOscPropertiesStruct SignalOscProperties;
        SignalOscProperties.Type = ST_DIGITAL;
        SignalOscProperties.LeftAxisIndex = 0;
        while (count < DigitalGraphNum)
        {
            QString tmps = DigitalDescription.Names.at(count);
            if (count < MAXGRAPHSPERPLOT)
            {
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
                DiscreteLegend->addItem(new QCPPlottableLegendItem(DiscreteLegend, graph));
                SignalOscProperties.Graph = graph;
                SignalOscProperties.Visible = true;
            }
            else
            {
                SignalOscProperties.Graph = nullptr;
                SignalOscProperties.Visible = false;
            }
            SignalOscPropertiesMap[tmps] = SignalOscProperties;
            ++count;
        }
        QCPAxis *axis = DigitalAxisRect->axis(QCPAxis::atBottom);
        axis->setSelectableParts(QCPAxis::spAxis);
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
        AnalogAxisRect->addAxis(QCPAxis::atLeft);
        MainPlot->plotLayout()->addElement(MainPlotLayoutRow++, 0, AnalogAxisRect);
        AnalogLegend = SetLegend(MainPlotLayoutRow++);
        AnalogAxisRect->setBackground(QBrush(QColor(ACONFYCLR)));
        int count = 0;
        switch(TrendModel->idOsc)
        {
        case MT_ID85:
        {
            while (count < AnalogGraphNum)
            {
                QString tmps = AnalogDescription.Names.at(count);
                SignalOscPropertiesStruct SignalOscProperties;
                SignalOscProperties.Type = ST_ANALOG;
                if (count < MAXGRAPHSPERPLOT)
                {
                    QCPGraph *graph;
                    if (tmps.at(0) == "I")
                    {
                        graph = MainPlot->addGraph(AnalogAxisRect->axis(QCPAxis::atBottom), AnalogAxisRect->axis(QCPAxis::atLeft));
                        graph->valueAxis()->setLabel("I");
                        SignalOscProperties.Graph = graph;
                        SignalOscProperties.LeftAxisIndex = 0;
                        SignalOscProperties.Visible = true;
                        SignalOscPropertiesMap[tmps] = SignalOscProperties;
                    }
                    else
                    {
                        graph = MainPlot->addGraph(AnalogAxisRect->axis(QCPAxis::atBottom),  AnalogAxisRect->axis(QCPAxis::atLeft, 1));
                        graph->valueAxis()->setLabel("U");
                        SignalOscProperties.Graph = graph;
                        SignalOscProperties.LeftAxisIndex = 1;
                        SignalOscProperties.Visible = true;
                        SignalOscPropertiesMap[tmps] = SignalOscProperties;
                    }
                    if (!AnalogDescription.Colors[tmps].isEmpty())
                        pen.setColor(QColor(AnalogDescription.Colors[tmps]));
                    else
                        pen.setColor(QColor(qSin(count*1+1.2)*80+80, qSin(count*0.3+0)*80+80, qSin(count*0.3+1.5)*80+80));
                    graph->setPen(pen);
                    graph->valueAxis()->setRange(YMin, YMax);
                    graph->keyAxis()->setLabel("Time, ms");
                    graph->keyAxis()->setRange(XMin, XMax);
                    graph->setName(tmps);
                    AnalogLegend->addItem(new QCPPlottableLegendItem(AnalogLegend, graph));
                }
                else // count > MAXGRAPHSPERPLOT
                {
                    if (tmps.at(0) == "I")
                        SignalOscProperties.LeftAxisIndex = 0;
                    else
                        SignalOscProperties.LeftAxisIndex = 1;
                    SignalOscProperties.Graph = nullptr;
                    SignalOscProperties.Visible = false;
                    SignalOscPropertiesMap[tmps] = SignalOscProperties;
                }
                ++count;
            }
            break;
        }
        default:
        {
            SignalOscPropertiesStruct SignalOscProperties;
            SignalOscProperties.Type = ST_ANALOG;
            SignalOscProperties.LeftAxisIndex = 0;
            while (count < AnalogGraphNum)
            {
                QString tmps = AnalogDescription.Names.at(count);
                if (count < MAXGRAPHSPERPLOT)
                {
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
                    AnalogLegend->addItem(new QCPPlottableLegendItem(AnalogLegend, graph));
                    SignalOscProperties.Graph = graph;
                    SignalOscProperties.Visible = true;
                }
                SignalOscProperties.Graph = nullptr;
                SignalOscProperties.Visible = false;
                SignalOscPropertiesMap[tmps] = SignalOscProperties;
                ++count;
            }
            break;
        }
        }
        AnalogAxisRect->insetLayout()->setMargins(QMargins(12, 12, 12, 12));
        QCPAxis *axis = AnalogAxisRect->axis(QCPAxis::atBottom);
        axis->setSelectableParts(QCPAxis::spAxis);
        connect(AnalogAxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)),this,SLOT(AnalogRangeChanged(QCPRange)));

    }
    else
        NoAnalog = true;
    connect(MainPlot, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));
    connect(MainPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(MouseWheel()));
    connect(MainPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(MousePress()));
    Starting = false;
}
