#include <QVector>
#include "trendviewdialog.h"
#include "../publicclass.h"

TrendViewDialog::TrendViewDialog(QWidget *parent) : QDialog(parent)
{
    QString tmps = "QDialog {background-color: "+QString(MAINWINCLRA1)+";}";
    setStyleSheet(tmps);
    setAttribute(Qt::WA_DeleteOnClose);
    SetupUI();
}

TrendViewDialog::~TrendViewDialog()
{

}

void TrendViewDialog::Init(int GraphNum, int PointNum, float RangeMin, float RangeMax)
{
    int count = 0;
    MainData.resize(GraphNum);
    while (count < GraphNum)
    {
        Plot->addGraph();
        QVector np;
        np.resize(PointNum);
        MainData.append(np);
    }
    QVector xv;
    for (int i=0; i<PointNum; ++i)
        xv.append(i);
    MainData.insert(0, xv);
    Plot->xAxis->setRange(RangeMin, RangeMax);
}

void TrendViewDialog::SetPoint(int GraphNum, float PointValue)
{
    if (GraphNum < MainData.size())
        MainData.at(GraphNum).append(PointValue);
}

void TrendViewDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    Plot = new QCustomPlot;
    lyout->addWidget(Plot);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(close()));
    hlyout->addStretch(10);
    hlyout->addWidget(pb);
    hlyout->addStretch(10);
    lyout->addLayout(hlyout);
    setLayout(lyout);
}

void TrendViewDialog::PlotShow()
{
    Plot->xAxis->setLabel("Points");
    int count = 0;
    while (count < GraphNum)
    {
        Plot->graph(count)->setData(MainData.at(0), MainData.at(count));
    }
    // give the axes some labels:
    Plot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    Plot->xAxis->setRange(-1, 1);
    Plot->yAxis->setRange(0, 1);
    Plot->replot();
}
