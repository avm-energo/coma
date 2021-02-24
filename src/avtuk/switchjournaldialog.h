#ifndef SWITCHJOURNALDIALOG_H
#define SWITCHJOURNALDIALOG_H

#include "../models/etablemodel.h"
#include "../widgets/etableview.h"
#include "../widgets/udialog.h"
#include "eoscillogram.h"
#include "swjdialog.h"

#include <QDialog>
#include <QModelIndex>

constexpr int MAXSWJNUM = 262144;

class SwitchJournalDialog : public UDialog
{
    Q_OBJECT
public:
    SwitchJournalDialog(EOscillogram *osc, QWidget *parent = nullptr);

private:
    ETableModel *TableModel;
    ETableView *SwjTableView;
    QMap<quint64, DataTypes::OscInfo> OscMap;
    QMap<int, S2DataTypes::SwitchJourInfo> SWJMap;
    EOscillogram *SWJDOscFunc;

    void SetupUI();
    void ProcessSWJournal(QByteArray &ba);
#if PROGSIZE != PROGSIZE_EMUL
    void ProcessOscillograms();

private slots:
    void LoadJournals();
    void ShowJournal(QModelIndex idx);
    void EraseJournals();
#endif
};

#endif // SWITCHJOURNALDIALOG_H
