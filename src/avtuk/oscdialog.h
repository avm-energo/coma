#ifndef OSCDIALOG_H
#define OSCDIALOG_H

#include "../models/etablemodel.h"
#include "../widgets/udialog.h"
#include "eoscillogram.h"

#include <QByteArray>
#include <QDialog>

class OscDialog : public UDialog
{
    Q_OBJECT

public:
    explicit OscDialog(QWidget *parent = nullptr);
    ~OscDialog();

private:
    void SetupUI();

    ETableModel *tm;
    int counter = 0;

signals:
    void StopCheckTimer();

public slots:
    void fillOscInfo(DataTypes::OscInfo info);
private slots:

    void GetOsc(QModelIndex);
    void EraseOsc();
};

#endif // OSCDIALOG_H
