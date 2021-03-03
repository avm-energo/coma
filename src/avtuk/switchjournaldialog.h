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
public:
    SwitchJournalDialog(EOscillogram *osc, QWidget *parent = nullptr);
    explicit SwitchJournalDialog(QWidget *parent = nullptr);

private:
    ETableModel *TableModel;
    ETableView *SwjTableView;
    QMap<quint64, DataTypes::OscInfo> OscMap;
    QMap<int, S2DataTypes::SwitchJourInfo> SWJMap;
    EOscillogram *SWJDOscFunc;

    void SetupUI();
    void processSWJournal(QByteArray &ba);

    void processOscillograms();

public slots:
    void FillJour(const DataTypes::FileStruct &fs);

private slots:
    void loadJournals();
    void showJournal(QModelIndex idx);
    void eraseJournals();
};

#endif // SWITCHJOURNALDIALOG_H
