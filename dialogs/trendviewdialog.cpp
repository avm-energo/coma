#include <QVector>
#include <QPen>
#include <QAction>
#include "QtXlsx/xlsxdocument.h"
#include "../config/config.h"
#include "trendviewdialog.h"

TrendViewDialog::TrendViewDialog(QVector<QString> &DigitalTrendNames, QVector<QString> &AnalogTrendNames, \
                                 int PointNum, float XRangeMin, float XRangeMax, float YRangeMin, \
                                 float YRangeMax, QWidget *parent) : QDialog(parent)
{
    PointsNum = 0;
    QString tmps = "QDialog {background-color: "+QString(MAINWINCLRA1)+";}";
    setStyleSheet(tmps);
    setAttribute(Qt::WA_DeleteOnClose);
    NoDiscrete = NoAnalog = false;
    QPen pen;
    int row = 0;
    this->PointsNum = PointNum;
    int DigitalGraphNum = DigitalTrendNames.size();
    int AnalogGraphNum = AnalogTrendNames.size();

    AnalogPlot = new QCustomPlot;
    AnalogPlot->setBackground(QBrush(QColor(ACONFYCLR)));
    AnalogPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(AnalogPlot,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(AnalogPlotContextMenu(QPoint)));
    AnalogPlot->plotLayout()->clear();
    QCPLegend *legend = new QCPLegend;
    AnalogPlot->legend = legend;
    AnalogPlot->legend->setLayer("legend");
    AnalogPlot->legend->setVisible(true);
    AnalogPlot->legend->setFont(QFont("Helvetica", 8));
    AnalogPlot->setAutoAddPlottableToLegend(true);
    AnalogPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectLegend);

    if (DigitalGraphNum != 0)
    {
        DiscretePlot = new QCustomPlot;
        DiscretePlot->setBackground(QBrush(QColor(ACONFYCLR)));
        DiscretePlot->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(DiscretePlot,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(DiscretePlotContextMenu(QPoint)));
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
        DigitalMainData.resize(DigitalGraphNum);
        int count = 0;
        while (count < DigitalGraphNum)
        {
            QCPGraph *graph = DiscretePlot->addGraph(DigitalAxisRect->axis(QCPAxis::atBottom), DigitalAxisRect->axis(QCPAxis::atLeft));
            pen.setColor(QColor(qSin(count*1+1.2)*80+80, qSin(count*0.3+0)*80+80, qSin(count*0.3+1.5)*80+80));
            graph->setPen(pen);
            graph->valueAxis()->setRange(-1, 2);
            graph->keyAxis()->setLabel("Time, ns");
            graph->keyAxis()->setRange(XRangeMin, XRangeMax);
            graph->valueAxis()->setLabel(DigitalTrendNames.at(count));
            graph->setName(DigitalTrendNames.at(count));
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
        AnalogMainData.resize(AnalogGraphNum);
        int count = 0;
        while (count < AnalogGraphNum)
        {
            QCPGraph *graph = AnalogPlot->addGraph(AnalogAxisRect->axis(QCPAxis::atBottom), AnalogAxisRect->axis(QCPAxis::atLeft));
            pen.setColor(QColor(qSin(count*1+1.2)*80+80, qSin(count*0.3+0)*80+80, qSin(count*0.3+1.5)*80+80));
            graph->setPen(pen);
            graph->valueAxis()->setRange(YRangeMin, YRangeMax);
            graph->keyAxis()->setLabel("Time, ns");
            graph->keyAxis()->setRange(XRangeMin, XRangeMax);
            graph->valueAxis()->setLabel(AnalogTrendNames.at(count));
            graph->setName(AnalogTrendNames.at(count));
            AnalogGraphs.append(graph);
            ++count;
        }
        AnalogAxisRect->insetLayout()->addElement(AnalogPlot->legend, Qt::AlignRight|Qt::AlignTop);
        AnalogAxisRect->insetLayout()->setMargins(QMargins(12, 12, 12, 12));
    }
    else
        NoAnalog = true;
    SetupUI();
}

TrendViewDialog::~TrendViewDialog()
{

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

void TrendViewDialog::SaveToExcel()
{
/*    if (Filename.isEmpty())
    {
        DBGMSG;
        return;
    }
    QStringList sl = Filename.split("#"); // отделяем имя файла от даты-времени
    if (sl.size() < 2)
    {
        DBGMSG;
        return;
    }
    Filename = sl.at(0);
    QString OscDateTime = sl.at(1);
    QString tmps = pc.HomeDir + "/";
    Filename.insert(0, tmps);
    QXlsx::Document xlsx(Filename);
    xlsx.write(1,1,QVariant("Модуль: "+pc.ModuleTypeString));
    xlsx.write(2,1,QVariant("Дата: "+OscDateTime.split(" ").at(0)));
    xlsx.write(3,1,QVariant("Время: "+OscDateTime.split(" ").at(1)));
    xlsx.write(4,1,QVariant("Смещение, мс"));
    quint32 MTypeB = pc.ModuleBsi.MTypeB << 8;
    switch(MTypeB)
    {
    case MTB_85:
    {
        for (int col = 2; col < 5; ++col)
        {
            QString phase = QString::number(8+col, 16); // A,B,C
            xlsx.write(4,col,QVariant("OCN"+phase));
            xlsx.write(4,col+3,QVariant("OCF"+phase));
            xlsx.write(4,col+6,QVariant("BKC"+phase));
            xlsx.write(4,col+9,QVariant("BKO"+phase));
            xlsx.write(4,col+14,QVariant("CN"+phase));
            xlsx.write(4,col+17,QVariant("CF"+phase));
            xlsx.write(4,col+20,QVariant("US"+phase));
            xlsx.write(4,col+23,QVariant("I"+phase));
            xlsx.write(4,col+26,QVariant("UL"+phase));
        }
        xlsx.write(4, 12, QVariant("CSC"));
        xlsx.write(4, 13, QVariant("CSO"));
        int row = 5;
        for (int i = 0; i < MainPoints.size(); ++i) // цикл по точкам
        {
            int col = 2; // 2 = OCNA
            for (int i=0; i<20; ++i)
            {
                if (DisPoint & 0x00000001)
                    xlsx.write(row, col++, QVariant("1"));
                else
                    xlsx.write(row, col++, QVariant("0"));
                DisPoint >>= 1;
            }
            col = 22;
            while (col < 31)
                xlsx.write(row, col++, QVariant(QString::number(point.An[col-22], 'f', 6)));
        break;
    }
    default:
        break;
    }
        xlsx.save(); */
}

void TrendViewDialog::SaveToComtrade()
{

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
    AnalogPlot->replot();
    DiscretePlot->replot();
    this->showMaximized();
}

bool TrendViewDialog::SetPointsAxis(float start, float step)
{
    if (step < 0)
    {
        WARNMSG("Ошибка в принятой осциллограмме, шаг отрицательный");
        return false;
    }
    if (PointsNum == 0)
        return false;
    MainPoints.clear();
    for (int i=0; i<PointsNum; ++i)
    {
        MainPoints << start;
        start += step;
    }
    return true;
}

void TrendViewDialog::SetFilename(const QString &fn)
{
    Filename = fn;
}
