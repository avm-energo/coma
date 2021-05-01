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

#include "trendviewdialog.h"

#include "../gen/colors.h"
#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/pch.h"
#include "../gen/stdfunc.h"
#include "../widgets/signalchoosewidget.h"
#include "../widgets/wd_func.h"
#include "parseid9050.h"

#include <QAction>
#include <QPen>
#include <QVector>
#include <algorithm>

TrendViewDialog::TrendViewDialog(QByteArray &ba, QWidget *parent) : QDialog(parent)
{
    // setAttribute(Qt::WA_DeleteOnClose);
    // setWindowFlags(Qt::WindowMinMaxButtonsHint);
    rangeChangeInProgress = false;
    rangeAxisInProgress = false;
    starting = true;
    m_arrayToSave = ba;
    analogRescaleActivated = false;
    digitalRescaleActivated = false;
}

TrendViewDialog::TrendViewDialog(QWidget *parent)
    : QDialog(parent)
    , rangeChangeInProgress(false)
    , starting(true)
    , rangeAxisInProgress(false)
    , digitalRescaleActivated(false)
    , analogRescaleActivated(false)
{
}

TrendViewDialog::~TrendViewDialog()
{
}

void TrendViewDialog::setupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QVBoxLayout *vlyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;

    if (!noDiscrete)
    {
        vlyout->addWidget(createToolBar(ST_DIGITAL));
        SignalChooseWidget *scw
            = new SignalChooseWidget(digitalDescription.names, digitalDescription.descriptions, this);
        scw->setObjectName("digital");
        // scw->setAccessibleDescription("Descr");
        connect(scw, &SignalChooseWidget::signalChoosed, this, &TrendViewDialog::signalChoosed);
        connect(scw, &SignalChooseWidget::signalToggled, this, &TrendViewDialog::signalToggled);
        vlyout->addWidget(scw);
    }
    if (!noAnalog)
    {
        vlyout->addWidget(createToolBar(ST_ANALOG));
        SignalChooseWidget *scw = new SignalChooseWidget(analogDescription.names, analogDescription.descriptions, this);
        scw->setObjectName("analog");
        connect(scw, &SignalChooseWidget::signalChoosed, this, &TrendViewDialog::signalChoosed);
        connect(scw, &SignalChooseWidget::signalToggled, this, &TrendViewDialog::signalToggled);
        vlyout->addWidget(scw);
    }
    hlyout->addLayout(vlyout);
    //    if (MainPlot.isNull())
    //    {
    //        qDebug(__PRETTY_FUNCTION__);
    //        return;
    //    }
    hlyout->addWidget(mainPlot.get(), 100);
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

QToolBar *TrendViewDialog::createToolBar(SignalTypes type)
{
    QToolBar *bar = new QToolBar(this);
    bar->setIconSize(QSize(20, 20));
    QAction *act = new QAction(bar);
    act->setCheckable(true);
    act->setData(type);
    act->setToolTip("Выбор осциллограмм");
    act->setIcon(QIcon(":/icons/resize.svg"));
    connect(act, &QAction::triggered, this, &TrendViewDialog::setRescale);
    bar->addAction(act);
    return bar;
}

void TrendViewDialog::changeRange(QCPRange range)
{
    if (rangeChangeInProgress || starting)
    {
        rangeChangeInProgress = false;
        return;
    }
    rangeChangeInProgress = true;
    QList<QCPAxisRect *> axisrects = mainPlot->axisRects();
    for (QCPAxisRect *rect : axisrects)
        rect->axis(QCPAxis::atBottom)->setRange(range);
    mainPlot->replot();
}

QCPLegend *TrendViewDialog::createLegend(int rectindex)
{
    // setup an extra legend for that axis rect:
    QCPLegend *rectlegend = new QCPLegend;
    // now we move the legend from the inset layout of the axis rect into the main grid layout.
    // We create a sub layout so we can generate a small gap between the plot layout cell border
    // and the legend border:
    QCPLayoutGrid *subLayout = new QCPLayoutGrid;
    mainPlot->plotLayout()->addElement(rectindex, 0, subLayout);
    subLayout->setMargins(QMargins(5, 0, 5, 5));
    subLayout->addElement(0, 0, rectlegend);
    // change the fill order of the legend, so it's filled left to right in columns:
    rectlegend->setFillOrder(QCPLegend::foColumnsFirst);
    // set legend's row stretch factor very small so it ends up with minimum height:
    mainPlot->plotLayout()->setRowStretchFactor(rectindex, 0.001);
    rectlegend->setLayer("legend");
    return rectlegend;
}

int TrendViewDialog::visibleSignalOscDescriptionSize(TrendViewDialog::SignalTypes type)
{
    int count = 0;
    for (SignalOscPropertiesStruct st : signalOscPropertiesMap)
    {
        if ((st.type == type) && (st.isVisible == true))
            ++count;
    }
    return count;
}

void TrendViewDialog::graphClicked(QCPAbstractPlottable *plot, int dataIndex)
{
    double dataValue = plot->interface1D()->dataMainValue(dataIndex);
    QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.")
                          .arg(plot->name())
                          .arg(dataIndex)
                          .arg(dataValue);
}

void TrendViewDialog::signalChoosed(QString signame)
{
    if (signalOscPropertiesMap.contains(signame))
    {
        QCPGraph *graph = (signalOscPropertiesMap.value(signame).graph);
        if (graph == nullptr)
            return;
        graph->setSelection(QCPDataSelection(QCPDataRange()));
    }
}

void TrendViewDialog::signalToggled(QString signame, bool isChecked)
{
    if (signalOscPropertiesMap.contains(signame))
    {
        if (!isChecked) // signal to be deleted from plot
        {
            if (signalOscPropertiesMap.value(signame).isVisible)
            {
                QCPGraph *graph = signalOscPropertiesMap.value(signame).graph;
                if (graph == nullptr)
                    return;
                mainPlot->removeGraph(graph);
                signalOscPropertiesMap[signame].graph = nullptr;
                SignalTypes st = signalOscPropertiesMap.value(signame).type;
                if (st == ST_ANALOG)
                    analogLegend->remove(analogLegend->itemWithPlottable(graph));
                else if (st == ST_DIGITAL)
                    discreteLegend->remove(discreteLegend->itemWithPlottable(graph));
                else
                    qDebug(__PRETTY_FUNCTION__);
                signalOscPropertiesMap[signame].isVisible = false;
            }
        }
        else // signal to be added to plot
        {
            if (signalOscPropertiesMap[signame].isVisible) // visible already
                return;
            SignalChooseWidget *scw = this->findChild<SignalChooseWidget *>("digital");
            int analogcount = visibleSignalOscDescriptionSize(ST_ANALOG);
            int digitalcount = visibleSignalOscDescriptionSize(ST_DIGITAL);
            if (((signalOscPropertiesMap[signame].type == ST_DIGITAL) && (digitalcount < MAXGRAPHSPERPLOT))
                || ((signalOscPropertiesMap[signame].type == ST_ANALOG) && (analogcount < MAXGRAPHSPERPLOT)))
            {
                if (scw != nullptr)
                    scw->setChecked(signame, true);
                QCPGraph *graph = signalOscPropertiesMap[signame].graph;
                if (graph == nullptr)
                {
                    int count = mainPlot->graphCount();
                    SignalTypes type = signalOscPropertiesMap[signame].type;
                    QCPAxisRect *AxisRect = nullptr;
                    if ((m_trendModel->idOsc >= 10001) && (m_trendModel->idOsc <= 10008))
                        AxisRect = mainPlot->axisRect(0);
                    else
                        AxisRect = (type == ST_ANALOG) ? mainPlot->axisRect(1) : mainPlot->axisRect(0);
                    int axisindex = (signame.at(0) == 'I') ? 0 : 1;
                    QCPGraph *graph = mainPlot->addGraph(
                        AxisRect->axis(QCPAxis::atBottom), AxisRect->axis(QCPAxis::atLeft, axisindex));
                    signalOscPropertiesMap[signame].isVisible = true;
                    signalOscPropertiesMap[signame].graph = graph;
                    QPen pen;
                    /*                graph->keyAxis()->setLabel("Time, ms");
                                    graph->keyAxis()->setRange(XMin, XMax);
                                    graph->valueAxis()->setLabel(graphname); */
                    graph->setName(signame);
                    if (type == ST_DIGITAL)
                    {
                        graph->setData(m_trendModel->MainPoints, m_trendModel->DigitalMainData[signame]);
                        graph->setLineStyle(QCPGraph::lsStepLeft); // импульсы
                        discreteLegend->addItem(new QCPPlottableLegendItem(discreteLegend, graph));
                        if (!digitalDescription.colors[signame].isEmpty())
                            pen.setColor(QColor(digitalDescription.colors[signame]));
                        else
                            pen.setColor(QColor(qSin(count * 1 + 1.2) * 80 + 80, qSin(count * 0.3 + 0) * 80 + 80,
                                qSin(count * 0.3 + 1.5) * 80 + 80));
                    }
                    else
                    {
                        /*                    double min =
                           *std::min_element(TrendModel->AnalogMainData[graphname].constBegin(), \
                                                                           TrendModel->AnalogMainData[graphname].constEnd());
                                            double max =
                           *std::max_element(TrendModel->AnalogMainData[graphname].constBegin(), \
                                                                           TrendModel->AnalogMainData[graphname].constEnd());
                                            graph->valueAxis()->setRange(min, max); */
                        graph->setData(m_trendModel->MainPoints, m_trendModel->AnalogMainData[signame]);
                        analogLegend->addItem(new QCPPlottableLegendItem(analogLegend, graph));
                        if (!analogDescription.colors[signame].isEmpty())
                            pen.setColor(QColor(analogDescription.colors[signame]));
                        else
                            pen.setColor(QColor(qSin(count * 1 + 1.2) * 80 + 80, qSin(count * 0.3 + 0) * 80 + 80,
                                qSin(count * 0.3 + 1.5) * 80 + 80));
                    }
                    graph->setPen(pen);
                    //                graph->rescaleValueAxis(true, true);
                    //                graph->rescaleKeyAxis();
                }
            }
            else
                scw->setChecked(signame, false);
        }
        mainPlot->replot();
    }
}

void TrendViewDialog::digitalRangeChanged(QCPRange range)
{
    if (!noAnalog)
        changeRange(range);
}

void TrendViewDialog::mouseWheel()
{
    // https://stackoverflow.com/questions/36807980/qcustomplot-mouse-interaction-on-secondary-axis
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed
    QList<QCPAxisRect *> axisrects = mainPlot->axisRects();
    for (QCPAxisRect *rect : axisrects)
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
        {
            // rect->setRangeZoom(0); // deny zoom when none of axes selected
        }
        // check for automatic rescaling enabled
    }
    if (digitalRescaleActivated)
    {
        // don't know how to rescale digital axes
    }
    if (analogRescaleActivated)
    {
        // suppose that the analog rect has index 1
        QCPAxisRect *rect = mainPlot->axisRect(1);
        if (rect != nullptr)
        {
            autoResizeRange(rect, 0);
            autoResizeRange(rect, 1);
        }
    }
    mainPlot->replot();
}

void TrendViewDialog::mousePress()
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

void TrendViewDialog::analogRangeChanged(QCPRange range)
{
    if (!noDiscrete)
        changeRange(range);
}

void TrendViewDialog::saveToExcel()
{
    // TrendModel->SaveToExcel(this);
}

void TrendViewDialog::saveToComtrade()
{
}

void TrendViewDialog::saveToOsc()
{
    QString filename = WDFunc::ChooseFileForSave(this, "Файлы осциллограмм (*.osc)", "osc");
    Files::SaveToFile(filename, m_arrayToSave /*, BAToSave.size(*/);
}

void TrendViewDialog::setRescale(bool isChecked)
{
    QAction *act = qobject_cast<QAction *>(sender());
    if (act != nullptr)
    {
        int tmps = act->data().toInt();
        if (tmps == ST_ANALOG)
            analogRescaleActivated = isChecked;
        else
            digitalRescaleActivated = isChecked;
    }
}

void TrendViewDialog::autoResizeRange(QCPAxisRect *rect, int index)
{
    double min = qAbs(rect->axis(QCPAxis::atLeft, index)->range().lower);
    double max = qAbs(rect->axis(QCPAxis::atLeft, index)->range().upper);
    if (min < max)
        rect->axis(QCPAxis::atLeft, index)->setRange(-min, min);
    else
        rect->axis(QCPAxis::atLeft, index)->setRange(-max, max);
}

void TrendViewDialog::showPlot()
{
    if (!noAnalog)
    {
        for (auto it = signalOscPropertiesMap.begin(); it != signalOscPropertiesMap.end(); it++)
        {
            QString tmps = it.key();
            if (signalOscPropertiesMap[tmps].isVisible)
            {
                QCPGraph *graph = signalOscPropertiesMap[tmps].graph;
                if (graph == nullptr)
                    continue;
                if (m_trendModel->AContains(tmps))
                {
                    graph->setData(m_trendModel->MainPoints, m_trendModel->AnalogMainData[tmps]);
                    graph->rescaleAxes();
                    graph->rescaleValueAxis(true, true);
                    graph->rescaleKeyAxis();
                    SignalChooseWidget *scw = this->findChild<SignalChooseWidget *>("analog");
                    if (scw != nullptr)
                        scw->setChecked(tmps, true);
                }
                if (m_trendModel->DContains(tmps))
                {
                    graph->setData(m_trendModel->MainPoints, m_trendModel->DigitalMainData[tmps]);
                    graph->rescaleValueAxis(true, true);
                    graph->rescaleKeyAxis();
                    SignalChooseWidget *scw = this->findChild<SignalChooseWidget *>("digital");
                    if (scw != nullptr)
                        scw->setChecked(tmps, true);
                }
            }
        }
        /*        QCPItemTracer *itr = new QCPItemTracer(AnalogPlot);
                itr->position->setCoords(0, 0); */
    }
    QCPMarginGroup *group = new QCPMarginGroup(mainPlot.get());
    QCPAxisRect *rect = mainPlot->axisRect(0);
    if (rect != nullptr)
        rect->setMarginGroup(QCP::msLeft, group);
    switch (m_trendModel->idOsc)
    {
    case MT_ID85:
    {
        rect = mainPlot->axisRect(1);
        if (rect != nullptr)
            rect->setMarginGroup(QCP::msLeft, group);
        else
            delete group;
        break;
    }
    default:
        delete group;
        return;
    }
    mainPlot->replot();
    this->showMaximized();
}

void TrendViewDialog::setModel(std::unique_ptr<TrendViewModel> model)
{
    m_trendModel = std::move(model);
}

void TrendViewDialog::setRange(float XRangeMin, float XRangeMax, float YRangeMin, float YRangeMax)
{
    xMin = XRangeMin;
    xMax = XRangeMax;
    yMin = YRangeMin;
    yMax = YRangeMax;
}

void TrendViewDialog::setDigitalNames(QStringList &names)
{
    digitalDescription.names = names;
}

void TrendViewDialog::setAnalogNames(QStringList &names)
{
    analogDescription.names = names;
}

void TrendViewDialog::setAnalogDescriptions(QStringList &descr)
{
    analogDescription.descriptions = descr;
}

void TrendViewDialog::setDiscreteDescriptions(QStringList &descr)
{
    digitalDescription.descriptions = descr;
}

void TrendViewDialog::setTrendModel(std::unique_ptr<TrendViewModel> mdl)
{
    m_trendModel = std::move(mdl);
}

QByteArray TrendViewDialog::arrayToSave() const
{
    return m_arrayToSave;
}

void TrendViewDialog::setArrayToSave(const QByteArray &arrayToSave)
{
    m_arrayToSave = arrayToSave;
}

void TrendViewDialog::setDigitalColors(QStringList &colors)
{
    for (int i = 0; i < digitalDescription.names.size(); ++i)
    {
        QString tmps = digitalDescription.names.at(i);
        if (i < colors.size())
            digitalDescription.colors[tmps] = colors.at(i);
        else
            digitalDescription.colors[tmps] = "";
    }
}

void TrendViewDialog::setAnalogColors(QStringList &colors)
{
    for (int i = 0; i < analogDescription.names.size(); ++i)
    {
        QString tmps = analogDescription.names.at(i);
        if (i < colors.size())
            analogDescription.colors[tmps] = colors.at(i);
        else
            analogDescription.colors[tmps] = "";
    }
}

void TrendViewDialog::analogAxis(int &MainPlotLayoutRow)
{
    int AnalogGraphNum = analogDescription.names.size();
    QPen pen;
    QCPTextElement *title = new QCPTextElement(mainPlot.get());
    title->setText("Аналоговые сигналы");
    title->setFont(QFont("sans", 12, QFont::Bold));
    mainPlot->plotLayout()->addElement(
        MainPlotLayoutRow++, 0, title); // place the title in the empty cell we've just created
    QCPAxisRect *AnalogAxisRect = new QCPAxisRect(mainPlot.get());
    if (m_trendModel->idOsc == MT_ID85)
        AnalogAxisRect->addAxis(QCPAxis::atLeft);
    mainPlot->plotLayout()->addElement(MainPlotLayoutRow++, 0, AnalogAxisRect);
    analogLegend = createLegend(MainPlotLayoutRow++);
    int count = 0;
    switch (m_trendModel->idOsc)
    {
    case MT_ID85:
    {
        while (count < AnalogGraphNum)
        {
            QString tmps = analogDescription.names.at(count);
            SignalOscPropertiesStruct SignalOscProperties;
            SignalOscProperties.type = ST_ANALOG;
            if (count < MAXGRAPHSPERPLOT)
            {
                QCPGraph *graph;
                if (tmps.at(0) == 'I')
                {
                    graph = mainPlot->addGraph(
                        AnalogAxisRect->axis(QCPAxis::atBottom), AnalogAxisRect->axis(QCPAxis::atLeft));
                    graph->valueAxis()->setLabel("I");
                    SignalOscProperties.graph = graph;
                    SignalOscProperties.leftAxisIndex = 0;
                    SignalOscProperties.isVisible = true;
                    signalOscPropertiesMap[tmps] = SignalOscProperties;
                }
                else
                {
                    graph = mainPlot->addGraph(
                        AnalogAxisRect->axis(QCPAxis::atBottom), AnalogAxisRect->axis(QCPAxis::atLeft, 1));
                    graph->valueAxis()->setLabel("U");
                    SignalOscProperties.graph = graph;
                    SignalOscProperties.leftAxisIndex = 1;
                    SignalOscProperties.isVisible = true;
                    signalOscPropertiesMap[tmps] = SignalOscProperties;
                }
                if (!analogDescription.colors[tmps].isEmpty())
                    pen.setColor(QColor(analogDescription.colors[tmps]));
                else
                    pen.setColor(QColor(qSin(count * 1 + 1.2) * 80 + 80, qSin(count * 0.3 + 0) * 80 + 80,
                        qSin(count * 0.3 + 1.5) * 80 + 80));
                graph->setPen(pen);
                graph->valueAxis()->setRange(yMin, yMax);
                graph->keyAxis()->setLabel("Time, ms");
                graph->keyAxis()->setRange(xMin, xMax);
                graph->setName(tmps);
                analogLegend->addItem(new QCPPlottableLegendItem(analogLegend, graph));
            }
            else // count > MAXGRAPHSPERPLOT
            {
                if (tmps.at(0) == 'I')
                    SignalOscProperties.leftAxisIndex = 0;
                else
                    SignalOscProperties.leftAxisIndex = 1;
                SignalOscProperties.graph = nullptr;
                SignalOscProperties.isVisible = false;
                signalOscPropertiesMap[tmps] = SignalOscProperties;
            }
            ++count;
        }
        break;
    }
    default:
    {
        SignalOscPropertiesStruct SignalOscProperties;
        SignalOscProperties.type = ST_ANALOG;
        SignalOscProperties.leftAxisIndex = 0;
        while (count < AnalogGraphNum)
        {
            QString tmps = analogDescription.names.at(count);
            if (count < MAXGRAPHSPERPLOT)
            {
                QCPGraph *graph = mainPlot->addGraph(
                    AnalogAxisRect->axis(QCPAxis::atBottom), AnalogAxisRect->axis(QCPAxis::atLeft));
                if (!analogDescription.colors[tmps].isEmpty())
                    pen.setColor(QColor(analogDescription.colors[tmps]));
                else
                    pen.setColor(QColor(qSin(count * 1 + 1.2) * 80 + 80, qSin(count * 0.3 + 0) * 80 + 80,
                        qSin(count * 0.3 + 1.5) * 80 + 80));
                graph->setPen(pen);
                graph->valueAxis()->setRange(yMin, yMax);
                if ((m_trendModel->idOsc > ID_OSC_CH0) && (m_trendModel->idOsc < ID_OSC_CH7))
                {
                    graph->keyAxis()->setLabel("Time, mcs");
                }
                else
                {
                    graph->keyAxis()->setLabel("Time, ms");
                }
                graph->keyAxis()->setRange(xMin, xMax);
                //            graph->valueAxis()->setLabel(tmps);
                graph->setName(tmps);
                analogLegend->addItem(new QCPPlottableLegendItem(analogLegend, graph));
                SignalOscProperties.graph = graph;
                SignalOscProperties.isVisible = true;
            }
            SignalOscProperties.graph = nullptr;
            SignalOscProperties.isVisible = false;
            signalOscPropertiesMap[tmps] = SignalOscProperties;
            ++count;
        }
        break;
    }
    }
    AnalogAxisRect->insetLayout()->setMargins(QMargins(12, 12, 12, 12));
    QCPAxis *axis = AnalogAxisRect->axis(QCPAxis::atBottom);
    axis->setSelectableParts(QCPAxis::spAxis);
    connect(AnalogAxisRect->axis(QCPAxis::atBottom), qOverload<const QCPRange &>(&QCPAxis::rangeChanged), this,
        &TrendViewDialog::analogRangeChanged);
}

void TrendViewDialog::digitalAxis(int &MainPlotLayoutRow)
{
    int DigitalGraphNum = digitalDescription.names.size();
    QPen pen;
    QCPTextElement *title = new QCPTextElement(mainPlot.get());
    title->setText("Дискретные сигналы");
    // title->setFont(QFont("sans", 12, QFont::Bold));
    mainPlot->plotLayout()->addElement(
        MainPlotLayoutRow++, 0, title); // place the title in the empty cell we've just created
    QCPAxisRect *DigitalAxisRect = new QCPAxisRect(mainPlot.get());
    //        DiscreteRectIndex = RectIndex++;
    mainPlot->plotLayout()->addElement(MainPlotLayoutRow++, 0, DigitalAxisRect);
    discreteLegend = createLegend(MainPlotLayoutRow++);

    int count = 0;
    SignalOscPropertiesStruct SignalOscProperties;
    SignalOscProperties.type = ST_DIGITAL;
    SignalOscProperties.leftAxisIndex = 0;
    while (count < DigitalGraphNum)
    {
        QString tmps = digitalDescription.names.at(count);
        if (count < MAXGRAPHSPERPLOT)
        {
            QCPGraph *graph = mainPlot->addGraph(
                DigitalAxisRect->axis(QCPAxis::atBottom), DigitalAxisRect->axis(QCPAxis::atLeft, 0));
            if (!digitalDescription.colors[tmps].isEmpty())
                pen.setColor(QColor(digitalDescription.colors[tmps]));
            else
                pen.setColor(QColor(qSin(count * 1 + 1.2) * 80 + 80, qSin(count * 0.3 + 0) * 80 + 80,
                    qSin(count * 0.3 + 1.5) * 80 + 80));
            graph->setPen(pen);
            graph->valueAxis()->setRange(-1, 2);
            graph->keyAxis()->setLabel("Time, ms");
            graph->keyAxis()->setRange(xMin, xMax);
            //            graph->valueAxis()->setLabel(tmps);
            graph->setName(tmps);
            graph->setLineStyle(QCPGraph::lsStepLeft); // импульсы
            discreteLegend->addItem(new QCPPlottableLegendItem(discreteLegend, graph));
            SignalOscProperties.graph = graph;
            SignalOscProperties.isVisible = true;
        }
        else
        {
            SignalOscProperties.graph = nullptr;
            SignalOscProperties.isVisible = false;
        }
        signalOscPropertiesMap[tmps] = SignalOscProperties;
        ++count;
    }
    QCPAxis *axis = DigitalAxisRect->axis(QCPAxis::atBottom);
    axis->setSelectableParts(QCPAxis::spAxis);
    DigitalAxisRect->insetLayout()->setMargins(QMargins(12, 12, 12, 12));
    connect(DigitalAxisRect->axis(QCPAxis::atBottom), qOverload<const QCPRange &>(&QCPAxis::rangeChanged), this,
        &TrendViewDialog::digitalRangeChanged);
}

void TrendViewDialog::setupPlots()
{
    noDiscrete = noAnalog = false;

    mainPlot = std::unique_ptr<QCustomPlot>(new QCustomPlot);
    mainPlot->plotLayout()->clear();
    mainPlot->setInteractions(QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectAxes);
    mainPlot->setAutoAddPlottableToLegend(false);
    int MainPlotLayoutRow = 0;
    //    int RectIndex = 0;
    if (digitalDescription.names.size() != 0)
    {
        digitalAxis(MainPlotLayoutRow);
    }
    else
        noDiscrete = true;
    if (analogDescription.names.size() != 0)
    {
        analogAxis(MainPlotLayoutRow);
    }
    else
        noAnalog = true;
    connect(mainPlot.get(), &QCustomPlot::plottableClick, this, &TrendViewDialog::graphClicked);
    connect(mainPlot.get(), &QCustomPlot::mouseWheel, this, &TrendViewDialog::mouseWheel);
    connect(mainPlot.get(), &QCustomPlot::mousePress, this, &TrendViewDialog::mousePress);
    starting = false;
}
