#include <QVector>
#include "trendviewdialog.h"
#include "../publicclass.h"

TrendViewDialog::TrendViewDialog(QWidget *parent) : QDialog(parent)
{
    QString tmps = "QDialog {background-color: "+QString(MAINWINCLRA1)+";}";
    setStyleSheet(tmps);
    setAttribute(Qt::WA_DeleteOnClose);
//    SetupUI();
}

TrendViewDialog::~TrendViewDialog()
{

}

void TrendViewDialog::SetupUI()
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
//    Plot = new QCustomPlot;
//    lyout->addWidget(Plot);
    QPushButton *pb = new QPushButton("Готово");
    connect(pb,SIGNAL(clicked(bool)),this,SLOT(close()));
    hlyout->addStretch(10);
    hlyout->addWidget(pb);
    hlyout->addStretch(10);
    lyout->addLayout(hlyout);
    setLayout(lyout);
//    SetPlot();
}

void TrendViewDialog::SetPlot()
{
//    QVector<double> x, y; // initialize with entries 0..100
/*    for (int i=0; i<101; ++i)
    {
      x.append(i/50.0 - 1); // x goes from -1 to 1
      y.append(x[i]*x[i]); // let's plot a quadratic function
    } */
    // create graph and assign data to it:
//    Plot->addGraph();
//    Plot->graph(0)->setData(x, y);
    // give the axes some labels:
    Plot->xAxis->setLabel("x");
    Plot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    Plot->xAxis->setRange(-1, 1);
    Plot->yAxis->setRange(0, 1);
    Plot->replot();
}
