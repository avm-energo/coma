#ifndef OSCDIALOG_H
#define OSCDIALOG_H

#include "../widgets/udialog.h"
#include "oscmanager.h"

#include <optional>

class ETableModel;

class OscDialog : public UDialog
{
    Q_OBJECT
    using OscHeader = S2DataTypes::OscHeader;
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

private:
    void SetupUI();
    OscManager manager;
    ETableModel *tm;
    int counter = 0;

signals:
    void StopCheckTimer();

public slots:
    void fillOscInfo(S2DataTypes::OscInfo info);
    void fillOsc(const DataTypes::FileStruct file);
private slots:

    void GetOsc(const QModelIndex &);
    void EraseOsc();
};

#endif // OSCDIALOG_H
