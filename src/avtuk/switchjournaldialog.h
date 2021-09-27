#ifndef SWITCHJOURNALDIALOG_H
#define SWITCHJOURNALDIALOG_H

#include "../widgets/udialog.h"
#include "oscmanager.h"
#include "swjmanager.h"

#include <QModelIndex>

class ETableModel;
class ETableView;

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
    explicit SwitchJournalDialog(QWidget *parent = nullptr);

private:
    void SetupUI();
    void processSWJournal(QByteArray &ba);
    void processOscillograms();
    void FillJour(const DataTypes::FileStruct &fs);
    void fillSwJInfo(S2DataTypes::SwitchJourInfo swjInfo);
    void getSwJ(const QModelIndex &idx);
    void eraseJournals();

    const QStringList craftTypeList { "D", "G", "CB" };
    ETableModel *TableModel;
    ETableView *SwjTableView;
    QMap<int, S2DataTypes::SwitchJourInfo> swjMap;

    std::unique_ptr<TrendViewModel> oscModel;
    OscManager oscManager;
    SwjManager swjManager;

    quint32 reqSwJNum = 0;
};

#endif // SWITCHJOURNALDIALOG_H
