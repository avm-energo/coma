#ifndef SWITCHJOURNALDIALOG_H
#define SWITCHJOURNALDIALOG_H

#include "../models/etablemodel.h"
#include "../widgets/etableview.h"
#include "../widgets/udialog.h"
#include "eoscillogram.h"
#include "swjdialog.h"

#include <QDialog>
#include <QModelIndex>

class SwitchJournalDialog : public UDialog
{
    Q_OBJECT

    enum Column
    {
        number = 0,
        switchNumber = 1,
        datetime = 2,
        id = 3,
        switchType = 4,
        download = 5
    };

public:
    SwitchJournalDialog(EOscillogram *osc, QWidget *parent = nullptr);
    explicit SwitchJournalDialog(QWidget *parent = nullptr);

private:
    const QStringList craftTypeList { "D", "G", "CB" };
    ETableModel *TableModel;
    ETableView *SwjTableView;
    QMap<quint64, DataTypes::OscInfo> OscMap;
    QMap<int, S2DataTypes::SwitchJourInfo> SWJMap;
    EOscillogram *SWJDOscFunc;

    void SetupUI();
    void processSWJournal(QByteArray &ba);

    void processOscillograms();

    int currentRow = 0;
    quint32 reqSwJNum = 0;
public slots:
    void FillJour(const DataTypes::FileStruct &fs);
    void fillSwJInfo(S2DataTypes::SwitchJourInfo swjInfo);
    void getSwJ(const QModelIndex &idx);
private slots:
    void loadJournals();
    void showJournal(QModelIndex idx);
    void eraseJournals();
};

#endif // SWITCHJOURNALDIALOG_H
