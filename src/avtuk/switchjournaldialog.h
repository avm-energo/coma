#ifndef SWITCHJOURNALDIALOG_H
#define SWITCHJOURNALDIALOG_H

#include "../widgets/udialog.h"
#include "oscmanager.h"

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

public:
    void fillJour(const DataTypes::FileStruct &fs);
    void fillSwJInfo(S2DataTypes::SwitchJourInfo swjInfo);

private:
    void setupUI();
    void processSWJournal(QByteArray &ba);
    void processOscillograms();
    void getSwJ(const QModelIndex &idx);
    void eraseJournals();

    ETableModel *tableModel;
    ETableView *swjTableView;
    QMap<int, S2DataTypes::SwitchJourInfo> swjMap;
    std::vector<DataTypes::FileStruct> fileBuffer;
    std::size_t fileCounter = 0;

    std::unique_ptr<TrendViewModel> oscModel;
    OscManager oscManager;
    SwjModel swjModel;
    quint32 reqSwJNum = 0;
};

class SwitchJournalViewDialog : public QDialog
{
public:
    SwitchJournalViewDialog(SwjModel &swjModel, TrendViewModel *const oscModel, OscManager &oscManager);
    SwitchJournalViewDialog(
        SwjModel &swjModel, const std::unique_ptr<TrendViewModel> &oscModel, OscManager &oscManager);

private:
    QPushButton *create(SwjModel &swjModel);
};

#endif // SWITCHJOURNALDIALOG_H
