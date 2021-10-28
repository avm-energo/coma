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
    void getSwJ(const QModelIndex &idx);
    void eraseJournals();
    bool loadIfExist(quint32 size);
    QString filename(quint64 time) const;

    UniquePointer<ETableModel> tableModel;
    ETableView *swjTableView;
    QMap<quint32, S2DataTypes::SwitchJourInfo> swjMap;
    std::vector<DataTypes::FileStruct> fileBuffer;

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
