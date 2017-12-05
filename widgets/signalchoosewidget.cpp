#include <QVBoxLayout>
#include "../gen/publicclass.h"
#include "../widgets/wd_func.h"
#include "signalchoosewidget.h"

SignalChooseWidget::SignalChooseWidget(QStringList &snames, QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *lyout = new QVBoxLayout;
    QHBoxLayout *hlyout = new QHBoxLayout;
    QString tmps = "QDialog {background-color: "+QString(MAINWINCLRA1)+";}";
    setStyleSheet(tmps);
    for (int i=0; i<snames.size(); ++i)
    {
        hlyout->addWidget(WDFunc::NewChB(this, snames.at(i), ""), 0);

    }
}
