#include "plotdialog.h"

#include "../../qcustomplot/qcustomplot.h"
#include "../gen/datamanager.h"

constexpr double valueRadius = 0.9;
constexpr double textRadius = 0.8;
constexpr double headerIndentX = 0.2;
constexpr double headerIndentY = 0.1;

PlotDialog::PlotDialog(QWidget *parent) : UDialog(parent)
{
    setupUI();
    setFloatBdQuery({ { 2400, 7 } });
    connect(&DataManager::GetInstance(), &DataManager::floatReceived, this, &UWidget::updateFloatData,
        Qt::QueuedConnection);
}

static bool processPhase(GraphData &data, const DataTypes::FloatStruct &fl)
{
    if (data.contains(fl.sigAdr))
    {
        if (data.phase.isI(fl.sigAdr))
        {
            data.phase.I = fl.sigVal;
            return true;
        }
        else if (data.phase.isU(fl.sigAdr))
        {
            data.phase.U = fl.sigVal;
            return true;
        }
    }
    return false;
}

void PlotDialog::updateFloatData(const DataTypes::FloatStruct &fl)
{
    if (!processPhase(graphPhaseA, fl))
        if (!processPhase(graphPhaseB, fl))
            processPhase(graphPhaseC, fl);

    UWidget::updateFloatData(fl);
}

[[nodiscard]] QCPItemText *PlotDialog::createText(QCustomPlot *parent, const QString &name, const QString &text) const
{
    auto textLabel = new QCPItemText(parent);
    textLabel->setObjectName(name);

    textLabel->setText(text);
    textLabel->setTextAlignment(Qt::AlignRight);
    textLabel->setFont(QFont(parent->font().family(), 16));
    textLabel->position->setType(QCPItemPosition::ptAbsolute);
    textLabel->setClipToAxisRect(false);
    return textLabel;
}

[[nodiscard]] QCustomPlot *PlotDialog::createExample()
{
    QCustomPlot *examplePlot = new QCustomPlot(this);
    {
        examplePlot->setBackground(QColor(255, 255, 255, 100));
        examplePlot->setAttribute(Qt::WA_OpaquePaintEvent, false);
        QCPItemText *textLabel = new QCPItemText(examplePlot);
        textLabel->setText("Желаемая\n картина");
        textLabel->setFont(QFont(font().family(), 16));
        textLabel->position->setType(QCPItemPosition::ptViewportRatio);
        textLabel->setClipToAxisRect(false);

        textLabel->position->setCoords(headerIndentX, headerIndentY);

        examplePlot->plotLayout()->clear();

        auto angularAxis = new QCPPolarAxisAngular(examplePlot);

        examplePlot->plotLayout()->addElement(0, 0, angularAxis);

        angularAxis->setRangeDrag(false);
        angularAxis->setTickLabelMode(QCPPolarAxisAngular::lmUpright);

        angularAxis->radialAxis()->setTickLabelMode(QCPPolarAxisRadial::lmUpright);
        angularAxis->radialAxis()->setTickLabelRotation(0);
        angularAxis->radialAxis()->setAngle(45);

        angularAxis->grid()->setAngularPen(QPen(QColor(200, 200, 200), 0, Qt::SolidLine));

        QCPLegend legend;
        angularAxis->grid()->setSubGridType(QCPPolarGrid::gtAll);
        {
            QCPPolarGraph *graph = new QCPPolarGraph(angularAxis, angularAxis->radialAxis());
            graph->setScatterStyle(QCPScatterStyle::ssDiamond);
            QPen pen(QColor(255, 239, 0));

            pen.setWidth(3);
            graph->setPen(pen);
            graph->addData({ 90, 0, 0 }, { valueRadius, 0, valueRadius }, true);
        }
        {
            QCPPolarGraph *graph = new QCPPolarGraph(angularAxis, angularAxis->radialAxis());
            graph->setScatterStyle(QCPScatterStyle::ssDiamond);
            QPen pen(Qt::green);

            pen.setWidth(3);
            graph->setPen(pen);
            graph->addData({ -30, 0, 240 }, { valueRadius, 0, valueRadius }, true);
        }
        {
            QCPPolarGraph *graph = new QCPPolarGraph(angularAxis, angularAxis->radialAxis());
            graph->setScatterStyle(QCPScatterStyle::ssDiamond);
            QPen pen(Qt::red);

            pen.setWidth(3);
            graph->setPen(pen);
            graph->addData({ -150, 0, 120 }, { valueRadius, 0, valueRadius }, true);
        }

        angularAxis->setRange(-180, 180);
        angularAxis->setRangeReversed(true);
        angularAxis->radialAxis()->setRange(0, 1);
        examplePlot->replot();
    }
    return examplePlot;
}

void PlotDialog::updateGraph(const GraphData &graphData, char phaseLiteral) const
{
    graphData.graph->data().data()->clear();
    if (graphData.phase.isValid())
    {
        auto vec = graphData.phase.toVector();

#ifdef ANIMATED_PLOT
        vec.first() += counter;
        vec.last() += counter;
#endif

        graphData.graph->addData(vec, { valueRadius, 0, valueRadius }, true);

        {
            QString name = QString::number(*graphData.regs.cbegin());
            auto textLabel = customPlot->findChild<QCPItemText *>(name);
            if (!textLabel)
            {
                textLabel = createText(customPlot, name, QString::fromStdString({ 'U', phaseLiteral }));
            }
            auto point = graphData.graph->coordsToPixels(vec.first(), textRadius);
            textLabel->position->setCoords(point);
        }
        {
            QString name = QString::number(*(--graphData.regs.cend()));
            auto textLabel = customPlot->findChild<QCPItemText *>(name);
            if (!textLabel)
            {
                textLabel = createText(customPlot, name, QString::fromStdString({ 'I', phaseLiteral }));
            }
            auto point = graphData.graph->coordsToPixels(vec.last(), textRadius);
            textLabel->position->setCoords(point);
        }
    }
}

void PlotDialog::reqUpdate()
{
    if (!updatesEnabled())
        return;

    updateGraph(graphPhaseA, 'a');
    updateGraph(graphPhaseB, 'b');
    updateGraph(graphPhaseC, 'c');

#ifdef ANIMATED_PLOT
    ++counter;
#endif

    customPlot->replot();
    UDialog::reqUpdate();
}

[[nodiscard]] static inline GraphData createAxis(
    QCPPolarAxisAngular *angularAxis, const QColor &color, std::initializer_list<quint32> list)
{
    GraphData graph { new QCPPolarGraph(angularAxis, angularAxis->radialAxis()), list, {} };
    QCPPolarGraph *g1 = graph.graph;

    g1->setScatterStyle(QCPScatterStyle::ssDiamond);
    QPen g1Pen(color);
    g1Pen.setWidth(3);
    g1->setPen(g1Pen);
    return graph;
}

void PlotDialog::setupUI()
{
    QHBoxLayout *lyout = new QHBoxLayout;
    customPlot = new QCustomPlot(this);
    {
        customPlot->setBackground(QColor(255, 255, 255, 100));
        customPlot->setAttribute(Qt::WA_OpaquePaintEvent, false);
        QCPItemText *textLabel = new QCPItemText(customPlot);
        textLabel->setText("Реальная\n картина");
        textLabel->setFont(QFont(font().family(), 16));
        textLabel->position->setType(QCPItemPosition::ptViewportRatio);
        textLabel->setClipToAxisRect(false);

        textLabel->position->setCoords(headerIndentX, headerIndentY);

        customPlot->plotLayout()->clear();
        customPlot->setAutoAddPlottableToLegend(true);

        auto *angularAxis = new QCPPolarAxisAngular(customPlot);

        customPlot->plotLayout()->addElement(0, 0, angularAxis);

        angularAxis->setRangeDrag(false);
        angularAxis->setTickLabelMode(QCPPolarAxisAngular::lmUpright);

        angularAxis->radialAxis()->setTickLabelMode(QCPPolarAxisRadial::lmUpright);
        angularAxis->radialAxis()->setTickLabelRotation(0);
        angularAxis->radialAxis()->setAngle(45);

        angularAxis->grid()->setAngularPen(QPen(QColor(200, 200, 200), 0, Qt::SolidLine));

        angularAxis->grid()->setSubGridType(QCPPolarGrid::gtRadial);

        graphPhaseA = createAxis(angularAxis, QColor(255, 239, 0), { 2401, 2404 });

        graphPhaseB = createAxis(angularAxis, QColor(Qt::green), { 2402, 2405 });

        graphPhaseC = createAxis(angularAxis, QColor(Qt::red), { 2403, 2406 });

        angularAxis->setRange(-180, 180);
        angularAxis->setRangeReversed(true);
        angularAxis->radialAxis()->setRange(0, 1);

        lyout->addWidget(customPlot);
    }

    auto examplePlot = createExample();
    lyout->addWidget(examplePlot);

    setLayout(lyout);
}
