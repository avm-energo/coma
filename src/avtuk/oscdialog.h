#ifndef OSCDIALOG_H
#define OSCDIALOG_H

#include "../models/etablemodel.h"
#include "../widgets/udialog.h"
#include "eoscillogram.h"
#include "oscmanager.h"

#include <QByteArray>
#include <QDialog>

class OscDialog : public UDialog
{
    Q_OBJECT

    enum Column
    {
        number = 0,
        datetime = 1,
        id = 2,
        size = 3,
        download = 4
    };

public:
    explicit OscDialog(QWidget *parent = nullptr);
    ~OscDialog();

private:
    void SetupUI();
    OscManager manager;
    ETableModel *tm;
    int counter = 0;

signals:
    void StopCheckTimer();

public slots:
    void fillOscInfo(DataTypes::OscInfo info);
    void fillOsc(const DataTypes::FileStruct file);
private slots:

    void GetOsc(const QModelIndex &);
    void EraseOsc();
};

#endif // OSCDIALOG_H
