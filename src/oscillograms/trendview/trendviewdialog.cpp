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

#include "../../module/modules.h"
#include "../../widgets/epopup.h"
#include "../../widgets/signalchoosewidget.h"
#include "../../widgets/wd_func.h"

#include <QAction>
#include <QPen>
#include <QToolBar>
#include <algorithm>
#include <gen/comaexception.h>
#include <gen/error.h>
#include <gen/files.h>
#include <gen/pch.h>
#include <gen/stdfunc.h>

constexpr int VOLTAGE_AXIS_INDEX = 0;
constexpr int CURRENT_AXIS_INDEX = 1;

TrendViewDialog::TrendViewDialog(QWidget *parent)
    : QDialog(parent, Qt::Window), rangeChangeInProgress(false), starting(true), rangeAxisInProgress(false)
{
    analog.rescaleActivated = false;
    digital.rescaleActivated = false;
    analog.type = ST_ANALOG;
    digital.type = ST_DIGITAL;
    setAttribute(Qt::WA_DeleteOnClose);
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
    auto graph = signalOscPropertiesMap.value(signame).graph;
    if (graph == nullptr)
        return;
    auto st = signalOscPropertiesMap.value(signame).type;
    if (st == ST_ANALOG)
        analog.legend->remove(analog.legend->itemWithPlottable(graph));
    else if (st == ST_DIGITAL)
        digital.legend->remove(digital.legend->itemWithPlottable(graph));
    else
        qDebug(__PRETTY_FUNCTION__);
    mainPlot->removeGraph(graph);
    signalOscPropertiesMap[signame].isVisible = false;
    signalOscPropertiesMap[signame].graph = nullptr;
    mainPlot->replot();
}

void TrendViewDialog::addSig(QString signame)
{
    // visible already
    if (signalOscPropertiesMap.value(signame).isVisible)
        return;
    auto st = signalOscPropertiesMap.value(signame).type;
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
        // auto digitalcount = visibleSignalOscDescriptionSize(ST_DIGITAL);
        // if (digitalcount >= MAXGRAPHSPERPLOT)
        //{
        //    PlotOverloadMessage(scw, signame);
        //    return;
        //}

        scw->setChecked(signame, true);
        auto graph = signalOscPropertiesMap.value(signame).graph;
        if (graph == nullptr)
        {
            auto count = mainPlot->graphCount();
            auto axisRect = mainPlot->axisRect(0);
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

            auto newGraph = mainPlot->addGraph(bottomAxis, leftAxis);
            signalOscPropertiesMap[signame].isVisible = true;
            signalOscPropertiesMap[signame].graph = newGraph;
            newGraph->setName(signame);
            newGraph->setLineStyle(QCPGraph::lsStepLeft); // импульсы
            newGraph->setData(m_trendModel->mainPoints(), m_trendModel->digitalMainData().value(signame));
            helper(digital, count, newGraph, signame);
            mainPlot->replot();
        }
        else
            scw->setChecked(signame, false);
    }
    else if (signalOscPropertiesMap.value(signame).type == ST_ANALOG)
    {
        // auto analogcount = visibleSignalOscDescriptionSize(ST_ANALOG);
        // if (analogcount >= MAXGRAPHSPERPLOT)
        //{
        //    PlotOverloadMessage(scw, signame);
        //    return;
        //}

        scw->setChecked(signame, true);
        auto graph = signalOscPropertiesMap.value(signame).graph;
        if (graph == nullptr)
        {
            auto count = mainPlot->graphCount();
            QCPAxisRect *axisRect = nullptr;
            // if ((m_trendModel->idOsc() >= AVTUK_21::OSC_ID_MIN) && (m_trendModel->idOsc() <= 10008))
            if (m_trendModel->idOsc() != AVTUK_85::OSC_ID)
                axisRect = mainPlot->axisRect(0);
            else
                axisRect = mainPlot->axisRect(1);
            auto axisIndex = (signame.at(0) == 'I') ? CURRENT_AXIS_INDEX : VOLTAGE_AXIS_INDEX;
            auto leftAxis = axisRect->axis(QCPAxis::atLeft, axisIndex);
            auto bottomAxis = axisRect->axis(QCPAxis::atBottom);
            if (!leftAxis || !bottomAxis)
            {
                qCritical() << "Nullptr axis";
                Q_CHECK_PTR(leftAxis);
                Q_CHECK_PTR(bottomAxis);
                return;
            }

            auto newGraph = mainPlot->addGraph(bottomAxis, leftAxis);
            signalOscPropertiesMap[signame].isVisible = true;
            signalOscPropertiesMap[signame].graph = newGraph;
            newGraph->setName(signame);
            newGraph->setData(m_trendModel->mainPoints(), m_trendModel->analogMainData().value(signame));
            helper(analog, count, newGraph, signame);
            mainPlot->replot();
        }
        else
            scw->setChecked(signame, false);
    }
}

void TrendViewDialog::PlotOverloadMessage(SignalChooseWidget *scw, QString &sname)
{
    scw->setChecked(sname, false);
    EMessageBox::warning(this, "На плоскость добавлено максимальное число графиков.");
}

void TrendViewDialog::setupUI()
{
    auto vlyout = new QVBoxLayout;

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

    auto h1Layout = new QHBoxLayout;
    auto nameLabel = new QLabel("Selected plot: ", this);
    nameOutput = new QLineEdit(this);
    nameOutput->setReadOnly(true);
    h1Layout->addWidget(nameLabel);
    h1Layout->addWidget(nameOutput);
    vlyout->addLayout(h1Layout);

    auto h2Layout = new QHBoxLayout;
    auto valueLabel = new QLabel("Value: ", this);
    valueOutput = new QLineEdit(this);
    valueOutput->setReadOnly(true);
    h2Layout->addWidget(valueLabel);
    h2Layout->addWidget(valueOutput);
    vlyout->addLayout(h2Layout);

    auto h3Layout = new QHBoxLayout;
    auto timeLabel = new QLabel("Time: ", this);
    timeOutput = new QLineEdit(this);
    timeOutput->setReadOnly(true);
    h3Layout->addWidget(timeLabel);
    h3Layout->addWidget(timeOutput);
    vlyout->addLayout(h3Layout);

    auto sidebarWidget = new QWidget(this);
    sidebarWidget->setMinimumWidth(350);
    sidebarWidget->setLayout(vlyout);

    auto hlyout = new QHBoxLayout;
    auto splitter = new QSplitter(Qt::Horizontal, this);
    splitter->insertWidget(0, sidebarWidget);
    splitter->insertWidget(1, mainPlot.get());
    // splitter->setStretchFactor(0, 10);
    // splitter->setStretchFactor(1, 11);
    hlyout->addWidget(splitter);
    setLayout(hlyout);
    auto rect = QGuiApplication::primaryScreen()->size();
    setGeometry(0, 0, rect.width() / 2, rect.height() / 2);
    setWindowState(Qt::WindowMaximized);
}

QToolBar *TrendViewDialog::createToolBar(SignalTypes type)
{
    auto bar = new QToolBar(this);
    bar->setIconSize(QSize(20, 20));
    auto act = new QAction(bar);
    act->setCheckable(true);
    act->setData(type);
    act->setToolTip("Автоподстройка");
    act->setIcon(QIcon(":/icons/resize.svg"));
    connect(act, &QAction::triggered, this, &TrendViewDialog::setRescale);
    bar->addAction(act);
    if (type == SignalTypes::ST_ANALOG)
    {
        act = new QAction(bar);
        act->setToolTip("Экспорт в Excel");
        act->setIcon(QIcon(":/icons/excel.svg"));
        connect(act, &QAction::triggered, this, &TrendViewDialog::exportToExcel);
        bar->addAction(act);
    }
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
    auto axisrects = mainPlot->axisRects();
    for (auto rect : axisrects)
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
    auto rectlegend = new QCPLegend;
    // now we move the legend from the inset layout of the axis rect into the main grid layout.
    // We create a sub layout so we can generate a small gap between the plot layout cell border
    // and the legend border:
    auto subLayout = new QCPLayoutGrid;
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
    auto x = plot->interface1D()->dataMainKey(dataIndex);
    auto y = plot->interface1D()->dataMainValue(dataIndex);
    auto name = plot->name();
    auto message = QString("Clicked on graph '%1' at data point #%2 with poisition X:%3, Y:%4.")
                       .arg(name)
                       .arg(dataIndex)
                       .arg(x)
                       .arg(y);
    qWarning() << message;

    if (name == "digitalSelector" || name == "analogSelector")
        return;

    nameOutput->setText(name);
    valueOutput->setText(QString::number(y));
    timeOutput->setText(QString::number(x));

    QVector<double> xValues { x, x };
    if (!digital.noSignals())
    {
        QVector<double> yValues { sizeX.min, sizeX.max };
        if (digital.selection)
        {
            mainPlot->removeGraph(digital.selection);
            digital.selection = nullptr;
        }

        auto axisRect = mainPlot->axisRect(0);
        auto leftAxis = axisRect->axis(QCPAxis::atLeft, 0);
        auto bottomAxis = axisRect->axis(QCPAxis::atBottom);
        digital.selection = mainPlot->addGraph(bottomAxis, leftAxis);
        digital.selection->setLineStyle(QCPGraph::lsStepLeft);
        digital.selection->setName("digitalSelector");
        digital.selection->setPen(QPen(Qt::black));
        digital.selection->setAntialiased(true);
        digital.selection->setData(xValues, yValues);
        mainPlot->replot();
    }

    if (!analog.noSignals())
    {
        QVector<double> yValues { -2000, 2000 };
        if (analog.selection)
        {
            mainPlot->removeGraph(analog.selection);
            analog.selection = nullptr;
        }

        QCPAxisRect *axisRect = nullptr;
        if (m_trendModel->idOsc() != AVTUK_85::OSC_ID)
            axisRect = mainPlot->axisRect(0);
        else
            axisRect = mainPlot->axisRect(1);
        auto axisIndex = CURRENT_AXIS_INDEX;
        auto leftAxis = axisRect->axis(QCPAxis::atLeft, axisIndex);
        auto bottomAxis = axisRect->axis(QCPAxis::atBottom);
        analog.selection = mainPlot->addGraph(bottomAxis, leftAxis);
        analog.selection->setLineStyle(QCPGraph::lsLine);
        analog.selection->setName("analogSelector");
        analog.selection->setPen(QPen(Qt::black));
        analog.selection->setAntialiased(true);
        analog.selection->setData(xValues, yValues);
        mainPlot->replot();
    }
}

void TrendViewDialog::signalChoosed(QString signame) const
{
    if (signalOscPropertiesMap.contains(signame))
    {
        auto graph = signalOscPropertiesMap.value(signame).graph;
        if (graph != nullptr)
            graph->setSelection(QCPDataSelection(QCPDataRange()));
    }
}

void TrendViewDialog::signalToggled(QString signame, bool isChecked)
{
    if (signalOscPropertiesMap.contains(signame))
    {
        if (!isChecked)
            removeSig(signame);
        else
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
    auto axisrects = mainPlot->axisRects();
    for (auto rect : axisrects)
    {
        auto axis = rect->axis(QCPAxis::atBottom, 0);
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

void TrendViewDialog::exportToExcel()
{
    QFileDialog *dlg = new QFileDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setFileMode(QFileDialog::AnyFile);
    QString filename = dlg->getSaveFileName(this, "Сохранить файл", StdFunc::GetHomeDir(), "Excel files (*.xlsx)",
        nullptr, QFileDialog::DontUseNativeDialog);
    QFileInfo info(filename);
    StdFunc::SetHomeDir(info.absolutePath());
    dlg->close();
    m_trendModel->setFilename(filename);
    m_trendModel->toExcel();
    EMessageBox::information(this, "Файл создан успешно");
}

void TrendViewDialog::analogRangeChanged(const QCPRange &range)
{
    if (!digital.noSignals())
        changeRange(range);
}

void TrendViewDialog::setRescale(bool isChecked)
{
    auto act = qobject_cast<QAction *>(sender());
    if (act != nullptr)
    {
        auto tmps = act->data().toInt();
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
    auto min = qAbs(axis->range().lower);
    auto max = qAbs(axis->range().upper);
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
            auto tmps = it.key();
            if (!signalOscPropertiesMap.value(tmps).isVisible)
                continue;

            auto graph = signalOscPropertiesMap.value(tmps).graph;
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

    auto group = new QCPMarginGroup(mainPlot.get());
    auto rect = mainPlot->axisRect(0);
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
    if (model != nullptr)
    {
        m_trendModel = model;
        digital.description.names = m_trendModel->digitalValues();
        analog.description.names = m_trendModel->analogValues();
        digital.description.descriptions = m_trendModel->digitalDescriptions();
        analog.description.descriptions = m_trendModel->analogDescriptions();
        digital.description.enableFlags = m_trendModel->digitalEnableFlags();
        analog.description.enableFlags = m_trendModel->analogEnableFlags();
        setDigitalColors(m_trendModel->digitalColors());
        setAnalogColors(m_trendModel->analogColors());
    }
}

void TrendViewDialog::setRange(float XRangeMin, float XRangeMax, float YRangeMin, float YRangeMax)
{
    sizeX.min = XRangeMin;
    sizeX.max = XRangeMax;
    sizeY.min = YRangeMin;
    sizeY.max = YRangeMax;
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
    auto title = new QCPTextElement(mainPlot.get());
    title->setText("Аналоговые сигналы");

    // place the title in the empty cell we've just created
    mainPlot->plotLayout()->addElement(MainPlotLayoutRow++, 0, title);
    auto AnalogAxisRect = new QCPAxisRect(mainPlot.get());
    AnalogAxisRect->addAxis(QCPAxis::atLeft);
    mainPlot->plotLayout()->addElement(MainPlotLayoutRow++, 0, AnalogAxisRect);
    analog.legend = createLegend(MainPlotLayoutRow++);
    analogAxisDefault(graphNum, AnalogAxisRect);
    AnalogAxisRect->insetLayout()->setMargins(QMargins(12, 12, 12, 12));
    auto axis = AnalogAxisRect->axis(QCPAxis::atBottom);
    axis->setSelectableParts(QCPAxis::spAxis);
    connect(AnalogAxisRect->axis(QCPAxis::atBottom),               //
        qOverload<const QCPRange &>(&QCPAxis::rangeChanged), this, //
        &TrendViewDialog::analogRangeChanged);
}

void TrendViewDialog::analogAxisDefault(int graphNum, QCPAxisRect *axisRect)
{
    QPen pen;
    SignalOscPropertiesStruct SignalOscProperties;
    SignalOscProperties.type = ST_ANALOG;
    int count = 0;
    for (auto index = 0; index < graphNum; index++)
    {
        auto bitsetIndex = analog.description.enableFlags.size() - index - 1;
        auto flag = analog.description.enableFlags.test(bitsetIndex);
        auto name = analog.description.names.at(index);
        auto firstChar = name.at(0);
        auto axisIndex = (firstChar == 'I') ? CURRENT_AXIS_INDEX : VOLTAGE_AXIS_INDEX;
        SignalOscProperties.leftAxisIndex = axisIndex;
        if (flag && count < MAXGRAPHSPERPLOT)
        {
            auto leftAxis = axisRect->axis(QCPAxis::atLeft, axisIndex);
            auto bottomAxis = axisRect->axis(QCPAxis::atBottom);
            if (!leftAxis || !bottomAxis)
            {
                qCritical() << "Nullptr axis";
                Q_CHECK_PTR(leftAxis);
                Q_CHECK_PTR(bottomAxis);
                return;
            }
            // Creating new graph for plot
            auto graph = mainPlot->addGraph(bottomAxis, leftAxis);
            // Graph settings
            if (!analog.description.colors[name].isEmpty())
                pen.setColor(QColor(analog.description.colors[name]));
            else
                pen.setColor(QColor(qSin(index * 1 + 1.2) * 80 + 80, qSin(index * 0.3 + 0) * 80 + 80,
                    qSin(index * 0.3 + 1.5) * 80 + 80));
            graph->valueAxis()->setLabel(QString(firstChar));
            graph->setPen(pen);
            graph->valueAxis()->setRange(sizeY.min, sizeY.max);
            graph->keyAxis()->setLabel(m_trendModel->xAxisDescription());
            graph->keyAxis()->setRange(sizeX.min, sizeX.max);
            graph->setName(name);
            // Adding graph in legend of plot
            analog.legend->addItem(new QCPPlottableLegendItem(analog.legend, graph));
            SignalOscProperties.graph = graph;
            SignalOscProperties.isVisible = true;
            count++;
        }
        else
        {
            SignalOscProperties.graph = nullptr;
            SignalOscProperties.isVisible = false;
        }
        signalOscPropertiesMap[name] = SignalOscProperties;
    }
}

void TrendViewDialog::digitalAxis(int &MainPlotLayoutRow)
{
    auto graphNum = digital.description.names.size();
    QPen pen;
    auto title = new QCPTextElement(mainPlot.get());
    title->setText("Дискретные сигналы");

    // place the title in the empty cell we've just created
    mainPlot->plotLayout()->addElement(MainPlotLayoutRow++, 0, title);
    auto DigitalAxisRect = new QCPAxisRect(mainPlot.get());

    mainPlot->plotLayout()->addElement(MainPlotLayoutRow++, 0, DigitalAxisRect);
    digital.legend = createLegend(MainPlotLayoutRow++);
    int count = 0;
    SignalOscPropertiesStruct SignalOscProperties { ST_DIGITAL, 0, nullptr, false };
    for (auto index = 0; index < graphNum; index++)
    {
        auto bitsetIndex = digital.description.enableFlags.size() - index - 1;
        auto flag = digital.description.enableFlags.test(bitsetIndex);
        auto name = digital.description.names.at(index);
        if (flag && count < MAXGRAPHSPERPLOT)
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
            if (!digital.description.colors[name].isEmpty())
                pen.setColor(QColor(digital.description.colors[name]));
            else
                pen.setColor(QColor(qSin(index * 1 + 1.2) * 80 + 80, qSin(index * 0.3 + 0) * 80 + 80,
                    qSin(index * 0.3 + 1.5) * 80 + 80));
            graph->setPen(pen);
            graph->valueAxis()->setRange(-1, 2);
            // graph->keyAxis()->setLabel("Time, ms");
            graph->keyAxis()->setRange(sizeX.min, sizeX.max);
            graph->setName(name);
            digital.legend->addItem(new QCPPlottableLegendItem(digital.legend, graph));
            SignalOscProperties.graph = graph;
            SignalOscProperties.isVisible = true;
            graph->setLineStyle(QCPGraph::lsStepLeft); // импульсы
            count++;
        }
        else
        {
            SignalOscProperties.graph = nullptr;
            SignalOscProperties.isVisible = false;
        }
        signalOscPropertiesMap[name] = SignalOscProperties;
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
    mainPlot->setMouseTracking(true);
    mainPlot->plotLayout()->clear();
    mainPlot->setInteractions(QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectAxes);
    mainPlot->setAutoAddPlottableToLegend(false);
    int MainPlotLayoutRow = 0;

    if (!digital.noSignals())
        digitalAxis(MainPlotLayoutRow);

    if (!analog.noSignals())
        analogAxis(MainPlotLayoutRow);

    connect(mainPlot.get(), &QCustomPlot::plottableClick, this, &TrendViewDialog::graphClicked);
    connect(mainPlot.get(), &QCustomPlot::mouseWheel, this, &TrendViewDialog::mouseWheel);
    starting = false;
}
