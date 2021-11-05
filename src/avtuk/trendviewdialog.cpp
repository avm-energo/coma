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

#include "../gen/error.h"
#include "../gen/files.h"
#include "../gen/modules.h"
#include "../gen/pch.h"
#include "../gen/stdfunc.h"
#include "../widgets/signalchoosewidget.h"
#include "../widgets/wd_func.h"

#include <QAction>
#include <QPen>
#include <QToolBar>
#include <algorithm>

TrendViewDialog::TrendViewDialog(QWidget *parent)
    : QDialog(parent), rangeChangeInProgress(false), starting(true), rangeAxisInProgress(false)
{
    analog.rescaleActivated = false;
    digital.rescaleActivated = false;
    analog.type = ST_ANALOG;
    digital.type = ST_DIGITAL;
}

SignalChooseWidget *TrendViewDialog::setupHelper(const TrendViewDialog::Signals &sig)
{
    auto scw = new SignalChooseWidget(sig.description.names, sig.description.descriptions, this);
    scw->setObjectName(QVariant::fromValue(sig.type).toString());
    connect(scw, &SignalChooseWidget::signalChoosed, this, &TrendViewDialog::signalChoosed);
    connect(scw, &SignalChooseWidget::signalToggled, this, &TrendViewDialog::signalToggled);
    return scw;
}

void TrendViewDialog::removeSig(QString signame)
{
    if (!signalOscPropertiesMap.value(signame).isVisible)
        return;
    QCPGraph *graph = signalOscPropertiesMap.value(signame).graph;
    if (graph == nullptr)
        return;
    mainPlot->removeGraph(graph);
    signalOscPropertiesMap[signame].graph = nullptr;
    SignalTypes st = signalOscPropertiesMap.value(signame).type;
    if (st == ST_ANALOG)
        analog.legend->remove(analog.legend->itemWithPlottable(graph));
    else if (st == ST_DIGITAL)
        digital.legend->remove(digital.legend->itemWithPlottable(graph));
    else
        qDebug(__PRETTY_FUNCTION__);
    signalOscPropertiesMap[signame].isVisible = false;
    mainPlot->replot();
}

void TrendViewDialog::addSig(QString signame)
{
    // visible already
    if (signalOscPropertiesMap.value(signame).isVisible)
        return;
    SignalTypes st = signalOscPropertiesMap.value(signame).type;
    auto scw = findChild<SignalChooseWidget *>(QVariant::fromValue(st).toString());
    if (!scw)
        return;

    auto helper = [](const Signals &sig, int counter, QCPGraph *graph, QString name) {
        QPen pen;
        sig.legend->addItem(new QCPPlottableLegendItem(sig.legend, graph));
        if (!sig.description.colors.value(name).isEmpty())
            pen.setColor(QColor(sig.description.colors.value(name)));
        else
            pen.setColor(QColor(qSin(counter * 1 + 1.2) * 80 + 80, qSin(counter * 0.3 + 0) * 80 + 80,
                qSin(counter * 0.3 + 1.5) * 80 + 80));
        graph->setPen(pen);
    };

    if (signalOscPropertiesMap.value(signame).type == ST_DIGITAL)
    {
        auto digitalcount = visibleSignalOscDescriptionSize(ST_DIGITAL);
        if (digitalcount >= MAXGRAPHSPERPLOT)
            return;

        scw->setChecked(signame, true);
        QCPGraph *graph = signalOscPropertiesMap.value(signame).graph;
        if (graph == nullptr)
        {
            int count = mainPlot->graphCount();
            QCPAxisRect *axisRect = mainPlot->axisRect(0);
            // auto axisIndex = (signame.at(0) == 'I') ? 0 : 1;
            auto leftAxis = axisRect->axis(QCPAxis::atLeft, 0 /*axisIndex*/);
            auto bottomAxis = axisRect->axis(QCPAxis::atBottom);
            if (!leftAxis || !bottomAxis)
            {
                qCritical() << "Nullptr axis";
                Q_CHECK_PTR(leftAxis);
                Q_CHECK_PTR(bottomAxis);
                return;
            }
            auto graph = mainPlot->addGraph(bottomAxis, leftAxis);
            signalOscPropertiesMap[signame].isVisible = true;
            signalOscPropertiesMap[signame].graph = graph;

            graph->setName(signame);

            graph->setLineStyle(QCPGraph::lsStepLeft); // импульсы
            graph->setData(m_trendModel->mainPoints(), m_trendModel->digitalMainData().value(signame));

            helper(digital, count, graph, signame);
            mainPlot->replot();
        }
        else
            scw->setChecked(signame, false);
    }
    else if (signalOscPropertiesMap.value(signame).type == ST_ANALOG)
    {
        auto analogcount = visibleSignalOscDescriptionSize(ST_ANALOG);
        if (analogcount >= MAXGRAPHSPERPLOT)
            return;

        scw->setChecked(signame, true);
        QCPGraph *graph = signalOscPropertiesMap.value(signame).graph;
        if (graph == nullptr)
        {
            int count = mainPlot->graphCount();
            QCPAxisRect *axisRect = nullptr;
            if ((m_trendModel->idOsc() >= AVTUK_21::OSC_ID_MIN) && (m_trendModel->idOsc() <= 10008))
                axisRect = mainPlot->axisRect(0);
            else
                axisRect = mainPlot->axisRect(1);
            auto axisIndex = (signame.at(0) == 'I') ? 0 : 1;
            auto leftAxis = axisRect->axis(QCPAxis::atLeft, axisIndex);
            auto bottomAxis = axisRect->axis(QCPAxis::atBottom);
            if (!leftAxis || !bottomAxis)
            {
                qCritical() << "Nullptr axis";
                Q_CHECK_PTR(leftAxis);
                Q_CHECK_PTR(bottomAxis);
                return;
            }
            auto graph = mainPlot->addGraph(bottomAxis, leftAxis);

            signalOscPropertiesMap[signame].isVisible = true;
            signalOscPropertiesMap[signame].graph = graph;

            graph->setName(signame);

            graph->setData(m_trendModel->mainPoints(), m_trendModel->analogMainData().value(signame));
            helper(analog, count, graph, signame);
            mainPlot->replot();
        }
        else
            scw->setChecked(signame, false);
    }
}

void TrendViewDialog::setupUI()
{
    QVBoxLayout *vlyout = new QVBoxLayout;

    if (!digital.noSignals())
    {
        vlyout->addWidget(createToolBar(ST_DIGITAL));
        vlyout->addWidget(setupHelper(digital));
    }
    if (!analog.noSignals())
    {
        vlyout->addWidget(createToolBar(ST_ANALOG));
        vlyout->addWidget(setupHelper(analog));
    }
    QHBoxLayout *hlyout = new QHBoxLayout;
    hlyout->addLayout(vlyout);

    hlyout->addWidget(mainPlot.get(), 100);

    setLayout(hlyout);
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

void TrendViewDialog::changeRange(const QCPRange &range)
{
    if (rangeChangeInProgress || starting)
    {
        rangeChangeInProgress = false;
        return;
    }
    rangeChangeInProgress = true;
    QList<QCPAxisRect *> axisrects = mainPlot->axisRects();
    for (QCPAxisRect *rect : axisrects)
    {
        auto axis = rect->axis(QCPAxis::atBottom);
        Q_CHECK_PTR(axis);
        if (axis)
            axis->setRange(range);
    }
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
    subLayout->setMargins(QMargins(5, 0, 5, 0));
    subLayout->addElement(0, 0, rectlegend);
    // change the fill order of the legend, so it's filled left to right in columns:
    rectlegend->setFillOrder(QCPLegend::foColumnsFirst);
    // set legend's row stretch factor very small so it ends up with minimum height:
    mainPlot->plotLayout()->setRowStretchFactor(rectindex, 0.001);
    rectlegend->setLayer("legend");
    return rectlegend;
}

ptrdiff_t TrendViewDialog::visibleSignalOscDescriptionSize(TrendViewDialog::SignalTypes type) const
{
    return std::count_if(signalOscPropertiesMap.cbegin(), signalOscPropertiesMap.cend(),
        [type](auto &&st) { return ((st.type == type) && (st.isVisible == true)); });
}

void TrendViewDialog::graphClicked(QCPAbstractPlottable *plot, int dataIndex)
{
    double dataValue = plot->interface1D()->dataMainValue(dataIndex);
    QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.")
                          .arg(plot->name())
                          .arg(dataIndex)
                          .arg(dataValue);
    qDebug() << message;
}

void TrendViewDialog::signalChoosed(QString signame) const
{
    if (!signalOscPropertiesMap.contains(signame))
        return;
    QCPGraph *graph = (signalOscPropertiesMap.value(signame).graph);
    if (graph == nullptr)
        return;
    graph->setSelection(QCPDataSelection(QCPDataRange()));
}

void TrendViewDialog::signalToggled(QString signame, bool isChecked)
{
    if (!signalOscPropertiesMap.contains(signame))
        return;
    if (!isChecked)
    {
        removeSig(signame);
    }
    else
    {
        addSig(signame);
    }
}

void TrendViewDialog::digitalRangeChanged(const QCPRange &range)
{
    if (!analog.noSignals())
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
    if (digital.rescaleActivated)
    {
        // don't know how to rescale digital axes
    }
    if (analog.rescaleActivated)
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

void TrendViewDialog::analogRangeChanged(const QCPRange &range)
{
    if (!digital.noSignals())
        changeRange(range);
}

void TrendViewDialog::setRescale(bool isChecked)
{
    QAction *act = qobject_cast<QAction *>(sender());
    if (act != nullptr)
    {
        int tmps = act->data().toInt();
        if (tmps == ST_ANALOG)
            analog.rescaleActivated = isChecked;
        else
            digital.rescaleActivated = isChecked;
    }
}

void TrendViewDialog::autoResizeRange(QCPAxisRect *rect, int index) const
{
    auto axis = rect->axis(QCPAxis::atLeft, index);
    Q_CHECK_PTR(axis);
    if (!axis)
        return;
    double min = qAbs(axis->range().lower);
    double max = qAbs(axis->range().upper);
    if (min < max)
        axis->setRange(-min, min);
    else
        axis->setRange(-max, max);
}

void TrendViewDialog::showAxes(QCPGraph *graph, const QVector<double> &keys, const QVector<double> &values,
    SignalTypes signalType, QString &widgetName)
{
    graph->setData(keys, values);
    graph->rescaleValueAxis(true, true);
    graph->rescaleKeyAxis();
    auto scw = findChild<SignalChooseWidget *>(QVariant::fromValue(signalType).toString());
    if (scw != nullptr)
        scw->setChecked(widgetName, true);
}

void TrendViewDialog::showPlot()
{
    if (!analog.noSignals())
    {
        for (auto it = signalOscPropertiesMap.begin(); it != signalOscPropertiesMap.end(); it++)
        {
            QString tmps = it.key();
            if (!signalOscPropertiesMap.value(tmps).isVisible)
                continue;

            QCPGraph *graph = signalOscPropertiesMap.value(tmps).graph;
            if (graph == nullptr)
                continue;
            if (m_trendModel->analogContains(tmps))
            {
                showAxes(
                    graph, m_trendModel->mainPoints(), m_trendModel->analogMainData().value(tmps), analog.type, tmps);
                graph->rescaleAxes();
            }
            if (m_trendModel->digitalContains(tmps))
            {
                showAxes(
                    graph, m_trendModel->mainPoints(), m_trendModel->digitalMainData().value(tmps), digital.type, tmps);
            }
        }
    }
    QCPMarginGroup *group = new QCPMarginGroup(mainPlot.get());
    QCPAxisRect *rect = mainPlot->axisRect(0);
    if (rect != nullptr)
        rect->setMarginGroup(QCP::msLeft, group);
    switch (m_trendModel->idOsc())
    {
    case AVTUK_85::OSC_ID:
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
}

void TrendViewDialog::setModel(TrendViewModel *model)
{
    m_trendModel = model;
}

void TrendViewDialog::setRange(float XRangeMin, float XRangeMax, float YRangeMin, float YRangeMax)
{
    sizeX.min = XRangeMin;
    sizeX.max = XRangeMax;
    sizeY.min = YRangeMin;
    sizeY.max = YRangeMax;
}

void TrendViewDialog::setDigitalNames(const QStringList &names)
{
    digital.description.names = names;
}

void TrendViewDialog::setAnalogNames(const QStringList &names)
{
    analog.description.names = names;
}

void TrendViewDialog::setAnalogDescriptions(const QStringList &descr)
{
    analog.description.descriptions = descr;
}

void TrendViewDialog::setDiscreteDescriptions(const QStringList &descr)
{
    digital.description.descriptions = descr;
}

void TrendViewDialog::setTrendModel(TrendViewModel *mdl)
{
    m_trendModel = mdl;
}

void TrendViewDialog::setDigitalColors(const QStringList &colors)
{

    for (int i = 0; i < digital.description.names.size(); ++i)
    {
        QString tmps = digital.description.names.at(i);
        if (i < colors.size())
            digital.description.colors[tmps] = colors.at(i);
        else
            digital.description.colors[tmps] = "";
    }
}

void TrendViewDialog::setAnalogColors(const QStringList &colors)
{
    for (int i = 0; i < analog.description.names.size(); ++i)
    {
        QString tmps = analog.description.names.at(i);
        if (i < colors.size())
            analog.description.colors[tmps] = colors.at(i);
        else
            analog.description.colors[tmps] = "";
    }
}

void TrendViewDialog::analogAxis(int &MainPlotLayoutRow)
{
    auto graphNum = analog.description.names.size();

    QCPTextElement *title = new QCPTextElement(mainPlot.get());
    title->setText("Аналоговые сигналы");
    // place the title in the empty cell we've just created
    mainPlot->plotLayout()->addElement(MainPlotLayoutRow++, 0, title);
    QCPAxisRect *AnalogAxisRect = new QCPAxisRect(mainPlot.get());
    if (m_trendModel->idOsc() == AVTUK_85::OSC_ID)
        AnalogAxisRect->addAxis(QCPAxis::atLeft);
    mainPlot->plotLayout()->addElement(MainPlotLayoutRow++, 0, AnalogAxisRect);
    analog.legend = createLegend(MainPlotLayoutRow++);
    switch (m_trendModel->idOsc())
    {
    case AVTUK_85::OSC_ID:
    {
        analogAxis85(graphNum, AnalogAxisRect);
        break;
    }
    default:
    {
        analogAxisDefault(graphNum, AnalogAxisRect);
        break;
    }
    }
    AnalogAxisRect->insetLayout()->setMargins(QMargins(12, 12, 12, 12));
    QCPAxis *axis = AnalogAxisRect->axis(QCPAxis::atBottom);
    axis->setSelectableParts(QCPAxis::spAxis);
    connect(AnalogAxisRect->axis(QCPAxis::atBottom), qOverload<const QCPRange &>(&QCPAxis::rangeChanged), this,
        &TrendViewDialog::analogRangeChanged);
}

void TrendViewDialog::analogAxisDefault(int graphNum, QCPAxisRect *axisRect)
{
    QPen pen;
    SignalOscPropertiesStruct SignalOscProperties;
    SignalOscProperties.type = ST_ANALOG;
    SignalOscProperties.leftAxisIndex = 0;
    for (auto count = 0; count < graphNum; count++)
    {
        QString tmps = analog.description.names.at(count);
        if (count < MAXGRAPHSPERPLOT)
        {
            auto leftAxis = axisRect->axis(QCPAxis::atLeft);
            auto bottomAxis = axisRect->axis(QCPAxis::atBottom);
            if (!leftAxis || !bottomAxis)
            {
                qCritical() << "Nullptr axis";
                Q_CHECK_PTR(leftAxis);
                Q_CHECK_PTR(bottomAxis);
                return;
            }
            auto graph = mainPlot->addGraph(bottomAxis, leftAxis);

            if (!analog.description.colors[tmps].isEmpty())
                pen.setColor(QColor(analog.description.colors[tmps]));
            else
                pen.setColor(QColor(qSin(count * 1 + 1.2) * 80 + 80, qSin(count * 0.3 + 0) * 80 + 80,
                    qSin(count * 0.3 + 1.5) * 80 + 80));
            graph->setPen(pen);

            graph->keyAxis()->setRange(sizeX.min, sizeX.max);

            graph->setName(tmps);
            analog.legend->addItem(new QCPPlottableLegendItem(analog.legend, graph));
            SignalOscProperties.graph = graph;
            SignalOscProperties.isVisible = true;

            graph->valueAxis()->setRange(sizeY.min, sizeY.max);
            if ((m_trendModel->idOsc() > ID_OSC_CH0) && (m_trendModel->idOsc() < ID_OSC_CH7))
            {
                graph->keyAxis()->setLabel("Time, mcs");
            }
            else
            {
                graph->keyAxis()->setLabel("Time, ms");
            }
        }
        SignalOscProperties.graph = nullptr;
        SignalOscProperties.isVisible = false;
        signalOscPropertiesMap[tmps] = SignalOscProperties;
    }
}

void TrendViewDialog::analogAxis85(int graphNum, QCPAxisRect *axisRect)
{
    for (auto count = 0; count < graphNum; count++)
    {
        QString tmps = analog.description.names.at(count);
        SignalOscPropertiesStruct SignalOscProperties;
        SignalOscProperties.type = ST_ANALOG;
        char firstChar = tmps.at(0).toLatin1();
        auto index = 0;
        switch (firstChar)
        {
        case 'I':
        {
            index = 0;
            break;
        }
        default:
        {
            index = 1;
        }
        }
        if (count < MAXGRAPHSPERPLOT)
        {

            auto leftAxis = axisRect->axis(QCPAxis::atLeft, index);
            auto bottomAxis = axisRect->axis(QCPAxis::atBottom);
            if (!leftAxis || !bottomAxis)
            {
                qCritical() << "Nullptr axis";
                Q_CHECK_PTR(leftAxis);
                Q_CHECK_PTR(bottomAxis);
                return;
            }
            auto graph = mainPlot->addGraph(bottomAxis, leftAxis);
            graph->valueAxis()->setLabel(QString(firstChar));
            SignalOscProperties.graph = graph;
            SignalOscProperties.leftAxisIndex = index;
            SignalOscProperties.isVisible = true;
            signalOscPropertiesMap[tmps] = SignalOscProperties;

            QPen pen;
            if (!analog.description.colors[tmps].isEmpty())
                pen.setColor(QColor(analog.description.colors[tmps]));
            else
                pen.setColor(QColor(qSin(count * 1 + 1.2) * 80 + 80, qSin(count * 0.3 + 0) * 80 + 80,
                    qSin(count * 0.3 + 1.5) * 80 + 80));
            graph->setPen(pen);
            graph->valueAxis()->setRange(sizeY.min, sizeY.max);
            graph->keyAxis()->setLabel("Time, ms");
            graph->keyAxis()->setRange(sizeX.min, sizeX.max);
            graph->setName(tmps);
            analog.legend->addItem(new QCPPlottableLegendItem(analog.legend, graph));
        }
        else // count > MAXGRAPHSPERPLOT
        {
            SignalOscProperties.leftAxisIndex = index;
            SignalOscProperties.graph = nullptr;
            SignalOscProperties.isVisible = false;
            signalOscPropertiesMap[tmps] = SignalOscProperties;
        }
    }
}

void TrendViewDialog::digitalAxis(int &MainPlotLayoutRow)
{
    auto graphNum = digital.description.names.size();
    QPen pen;
    QCPTextElement *title = new QCPTextElement(mainPlot.get());
    title->setText("Дискретные сигналы");

    mainPlot->plotLayout()->addElement(
        MainPlotLayoutRow++, 0, title); // place the title in the empty cell we've just created
    QCPAxisRect *DigitalAxisRect = new QCPAxisRect(mainPlot.get());

    mainPlot->plotLayout()->addElement(MainPlotLayoutRow++, 0, DigitalAxisRect);
    digital.legend = createLegend(MainPlotLayoutRow++);
    SignalOscPropertiesStruct SignalOscProperties { ST_DIGITAL, 0, nullptr, false };
    for (auto count = 0; count < graphNum; count++)
    {

        QString tmps = digital.description.names.at(count);
        if (count < MAXGRAPHSPERPLOT)
        {
            auto leftAxis = DigitalAxisRect->axis(QCPAxis::atLeft);
            auto bottomAxis = DigitalAxisRect->axis(QCPAxis::atBottom);
            if (!leftAxis || !bottomAxis)
            {
                qCritical() << "Nullptr axis";
                Q_CHECK_PTR(leftAxis);
                Q_CHECK_PTR(bottomAxis);
                return;
            }
            auto graph = mainPlot->addGraph(bottomAxis, leftAxis);
            if (!digital.description.colors[tmps].isEmpty())
                pen.setColor(QColor(digital.description.colors[tmps]));
            else
                pen.setColor(QColor(qSin(count * 1 + 1.2) * 80 + 80, qSin(count * 0.3 + 0) * 80 + 80,
                    qSin(count * 0.3 + 1.5) * 80 + 80));
            graph->setPen(pen);
            graph->valueAxis()->setRange(-1, 2);
            graph->keyAxis()->setLabel("Time, ms");
            graph->keyAxis()->setRange(sizeX.min, sizeX.max);

            graph->setName(tmps);
            digital.legend->addItem(new QCPPlottableLegendItem(digital.legend, graph));
            SignalOscProperties.graph = graph;
            SignalOscProperties.isVisible = true;

            graph->setLineStyle(QCPGraph::lsStepLeft); // импульсы
        }
        else
        {
            SignalOscProperties.graph = nullptr;
            SignalOscProperties.isVisible = false;
        }
        signalOscPropertiesMap[tmps] = SignalOscProperties;
    }
    auto axis = DigitalAxisRect->axis(QCPAxis::atBottom);
    Q_CHECK_PTR(axis);
    if (!axis)
        return;
    axis->setSelectableParts(QCPAxis::spAxis);
    DigitalAxisRect->insetLayout()->setMargins(QMargins(12, 12, 12, 12));
    connect(axis, qOverload<const QCPRange &>(&QCPAxis::rangeChanged), this, &TrendViewDialog::digitalRangeChanged);
}

void TrendViewDialog::setupPlots()
{

    mainPlot = std::unique_ptr<QCustomPlot>(new QCustomPlot);
    mainPlot->plotLayout()->clear();
    mainPlot->setInteractions(QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectAxes);
    mainPlot->setAutoAddPlottableToLegend(false);
    int MainPlotLayoutRow = 0;

    if (!digital.description.names.empty())
    {
        digitalAxis(MainPlotLayoutRow);
    }

    if (!analog.description.names.empty())
    {
        analogAxis(MainPlotLayoutRow);
    }

    connect(mainPlot.get(), &QCustomPlot::plottableClick, this, &TrendViewDialog::graphClicked);
    connect(mainPlot.get(), &QCustomPlot::mouseWheel, this, &TrendViewDialog::mouseWheel);

    starting = false;
}
