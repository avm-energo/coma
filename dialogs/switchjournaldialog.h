#ifndef SWITCHJOURNALDIALOG_H
#define SWITCHJOURNALDIALOG_H

#include <QDialog>
#include <QModelIndex>
#include "../gen/eoscillogram.h"
#include "../widgets/s_tqtableview.h"
#include "../widgets/etablemodel.h"
#include "swjdialog.h"

#define MAXSWJNUM   512

class SwitchJournalDialog : public QDialog
{
    Q_OBJECT
public:
    SwitchJournalDialog(QWidget *parent=0);

private:

    ETableModel *TableModel;
    s_tqTableView *SwjTableView;
    QMap<quint64, EOscillogram::GBoStruct> OscMap;
    QMap<quint32, SWJDialog::SWJournalRecordStruct> SWJMap;

    void SetupUI();
    void ProcessSWJournal(QByteArray &ba);
    void ProcessOscillograms();

private slots:
    void LoadJournals();
    void ShowJournal(QModelIndex idx);
    void EraseJournals();
};

#endif // SWITCHJOURNALDIALOG_H
