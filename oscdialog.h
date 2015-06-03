#ifndef OSCDIALOG_H
#define OSCDIALOG_H

#include <QDialog>
#include <QByteArray>

#include "widgets/s_tablemodel.h"

class oscdialog : public QDialog
{
    Q_OBJECT

public:
    explicit oscdialog(QWidget *parent=0);

private:
    void SetupUI();

    s_tablemodel *tm;
    QByteArray OscInfo;

signals:
    void endprogressbar();

private slots:
    void GetOscInfo();
    void ProcessOscInfo();
    void SetProgressBarSize(quint32);
    void SetProgressBar(quint32);
    void BeginExtractOsc(QModelIndex);
    void EndExtractOsc();
};

#endif // OSCDIALOG_H
