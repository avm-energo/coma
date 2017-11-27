#ifndef SWITCHJOURNAL_H
#define SWITCHJOURNAL_H

#include <QDialog>
#include <QModelIndex>
#include "../widgets/etablemodel.h"

class SwitchJournal : public QDialog
{
    Q_OBJECT
public:
    SwitchJournal(QWidget *parent=0);

private:
    ETableModel *TableModel;
    void SetupUI();

private slots:
    void LoadJournals();
    void ShowJournal(QModelIndex idx);
    void EraseJournals();
};

#endif // SWITCHJOURNAL_H
