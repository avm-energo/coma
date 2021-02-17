#ifndef SWITCHJOURNALDIALOG_H
#define SWITCHJOURNALDIALOG_H

#include <QDialog>
#include <QModelIndex>
//#include "../gen/maindef.h"
#include "../models/etablemodel.h"
#include "../widgets/etableview.h"
#include "eoscillogram.h"
#include "swjdialog.h"

#define MAXSWJNUM 262144

class SwitchJournalDialog : public QDialog
{
    Q_OBJECT
public:
    SwitchJournalDialog(EOscillogram *osc, QWidget *parent = nullptr);

private:
    ETableModel *TableModel;
    ETableView *SwjTableView;
    QMap<quint64, EOscillogram::GBoStruct> OscMap;
    QMap<int, SWJDialog::SWJINFStruct> SWJMap;
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
