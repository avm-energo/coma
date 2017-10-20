#ifndef TRENDVIEWDIALOG_H
#define TRENDVIEWDIALOG_H

#include <QDialog>
#include "../qcustomplot.h"

class TrendViewDialog : public QDialog
{
    Q_OBJECT
public:
    TrendViewDialog(QWidget *parent=0);
    ~TrendViewDialog();

private:
    QCustomPlot *Plot;

    void SetupUI();
    void SetPlot();
};

#endif // TRENDVIEWDIALOG_H
