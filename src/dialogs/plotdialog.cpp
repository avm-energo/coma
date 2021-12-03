#include "plotdialog.h"

#include "../gen/datamanager.h"

#include <../qcustomplot/qcustomplot.h>

constexpr double valueRadius = 0.9;
constexpr double textRadius = 0.8;
constexpr double headerIndentX = 0.15;
constexpr double headerIndentY = 0.05;

namespace originValues
{
constexpr auto Ua = 0;
constexpr auto Ia = 90;
constexpr auto Ub = -120;
constexpr auto Ib = -30;
constexpr auto Uc = 120;
constexpr auto Ic = -150;
}

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

static void addTextLabel(QString str, double xFactor, double yFactor, QCustomPlot *parent,
    QCPItemPosition::PositionType type = QCPItemPosition::ptAbsolute)
{
    QCPItemText *textLabel = new QCPItemText(parent);
    textLabel->setObjectName(str);
    textLabel->setText(str);
    textLabel->setFont(QFont(parent->font().family(), 16));
    textLabel->position->setType(type);
    textLabel->setClipToAxisRect(false);
    textLabel->position->setCoords(xFactor, yFactor);
}

static inline void addTextLabel(
    QString str, QPointF point, QCustomPlot *parent, QCPItemPosition::PositionType type = QCPItemPosition::ptAbsolute)
{
    addTextLabel(str, point.x(), point.y(), parent, type);
}

[[nodiscard]] QCustomPlot *PlotDialog::createExample()
{
    QCustomPlot *examplePlot = new QCustomPlot(this);
    {
        examplePlot->setBackground(QColor(255, 255, 255, 100));
        examplePlot->setAttribute(Qt::WA_OpaquePaintEvent, false);
        addTextLabel("Желаемая\n картина", headerIndentX, headerIndentY, examplePlot, QCPItemPosition::ptViewportRatio);

        examplePlot->plotLayout()->clear();

        exampleAngularAxis = new QCPPolarAxisAngular(examplePlot);

        examplePlot->plotLayout()->addElement(0, 0, exampleAngularAxis);

        exampleAngularAxis->setRangeDrag(false);
        exampleAngularAxis->radialAxis()->setTickLabels(false);
        exampleAngularAxis->radialAxis()->setAngle(0);
        exampleAngularAxis->grid()->setAngularPen(QPen(QColor(200, 200, 200), 0, Qt::SolidLine));
        exampleAngularAxis->grid()->setSubGridType(QCPPolarGrid::gtAll);

        using namespace originValues;

        {
            QCPPolarGraph *graph = new QCPPolarGraph(exampleAngularAxis, exampleAngularAxis->radialAxis());
            graph->setScatterStyle(QCPScatterStyle::ssDiamond);
            QPen pen(QColor(255, 239, 0));

            pen.setWidth(3);
            graph->setPen(pen);
            graph->addData({ Ia, 0, Ua }, { valueRadius, 0, valueRadius }, true);
        }
        {
            QCPPolarGraph *graph = new QCPPolarGraph(exampleAngularAxis, exampleAngularAxis->radialAxis());
            graph->setScatterStyle(QCPScatterStyle::ssDiamond);
            QPen pen(Qt::green);

            pen.setWidth(3);
            graph->setPen(pen);
            graph->addData({ Ib, 0, 360 + Ub }, { valueRadius, 0, valueRadius }, true);
        }
        {
            QCPPolarGraph *graph = new QCPPolarGraph(exampleAngularAxis, exampleAngularAxis->radialAxis());
            graph->setScatterStyle(QCPScatterStyle::ssDiamond);
            QPen pen(Qt::red);

            pen.setWidth(3);
            graph->setPen(pen);
            graph->addData({ Ic, 0, Uc }, { valueRadius, 0, valueRadius }, true);
        }

        exampleAngularAxis->setRange(-180, 180);
        exampleAngularAxis->setRangeReversed(true);
        exampleAngularAxis->radialAxis()->setRange(0, 1);

        {
            using namespace originValues;
            addTextLabel("Ua", exampleAngularAxis->coordToPixel(Ua, valueRadius), examplePlot);
            addTextLabel("Ia", exampleAngularAxis->coordToPixel(Ia, valueRadius), examplePlot);
            addTextLabel("Ub", exampleAngularAxis->coordToPixel(Ub, valueRadius), examplePlot);
            addTextLabel("Ib", exampleAngularAxis->coordToPixel(Ib, valueRadius), examplePlot);
            addTextLabel("Uc", exampleAngularAxis->coordToPixel(Uc, valueRadius), examplePlot);
            addTextLabel("Ic", exampleAngularAxis->coordToPixel(Ic, valueRadius), examplePlot);
        }

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

        angularAxis->radialAxis()->setAngle(0);

        angularAxis->grid()->setAngularPen(QPen(QColor(200, 200, 200), 0, Qt::SolidLine));

        angularAxis->grid()->setSubGridType(QCPPolarGrid::gtRadial);

        graphPhaseA = createAxis(angularAxis, QColor(255, 239, 0), { 2401, 2404 });

        graphPhaseB = createAxis(angularAxis, QColor(Qt::green), { 2402, 2405 });

        graphPhaseC = createAxis(angularAxis, QColor(Qt::red), { 2403, 2406 });

        angularAxis->setRange(-180, 180);
        angularAxis->setRangeReversed(true);
        angularAxis->radialAxis()->setRange(0, 1);

        angularAxis->radialAxis()->setTickLabels(false);

        lyout->addWidget(customPlot);
    }

    auto examplePlot = createExample();
    lyout->addWidget(examplePlot);

    setLayout(lyout);
}

void PlotDialog::paintEvent(QPaintEvent *event)
{
    UDialog::paintEvent(event);
    if (!exampleAngularAxis)
        return;

    using namespace originValues;
    auto textLabels = exampleAngularAxis->parentPlot()->findChildren<QCPItemText *>();
    bool needToReplot = false;
    for (auto &&label : qAsConst(textLabels))
    {
        if (label->objectName() == "Ua")
        {
            if (label->position->coords() != exampleAngularAxis->coordToPixel(Ua, valueRadius))
            {
                label->position->setCoords(exampleAngularAxis->coordToPixel(Ua, valueRadius));
                needToReplot = true;
            }
        }
        if (label->objectName() == "Ia")
        {
            if (label->position->coords() != exampleAngularAxis->coordToPixel(Ia, valueRadius))
            {
                label->position->setCoords(exampleAngularAxis->coordToPixel(Ia, valueRadius));
                needToReplot = true;
            }
        }
        if (label->objectName() == "Ub")
        {
            if (label->position->coords() != exampleAngularAxis->coordToPixel(Ub, valueRadius))
            {
                label->position->setCoords(exampleAngularAxis->coordToPixel(Ub, valueRadius));
                needToReplot = true;
            }
        }
        if (label->objectName() == "Ib")
        {
            if (label->position->coords() != exampleAngularAxis->coordToPixel(Ib, valueRadius))
            {
                label->position->setCoords(exampleAngularAxis->coordToPixel(Ib, valueRadius));
                needToReplot = true;
            }
        }
        if (label->objectName() == "Uc")
        {
            if (label->position->coords() != exampleAngularAxis->coordToPixel(Uc, valueRadius))
            {
                label->position->setCoords(exampleAngularAxis->coordToPixel(Uc, valueRadius));
                needToReplot = true;
            }
        }
        if (label->objectName() == "Ic")
        {
            if (label->position->coords() != exampleAngularAxis->coordToPixel(Ic, valueRadius))
            {
                label->position->setCoords(exampleAngularAxis->coordToPixel(Ic, valueRadius));
                needToReplot = true;
            }
        }
    }
    if (needToReplot)
        exampleAngularAxis->parentPlot()->replot(QCustomPlot::rpQueuedReplot);
}
